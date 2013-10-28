#include <stdio.h>
#include <cuda_runtime.h>

// ===========================================================================================================
// Important:   due to the use of warp shuffling, the C version of the backtransformation kernel only works on 
//              devices with compute capability 3.x; for older devices, please use the Fortran kernel version
// ===========================================================================================================

// Perform the equivalent of "__shfl_xor" on an 8-byte value
static __device__ __forceinline__ double shfl_xor(double r, int mask)
{
    int hi = __shfl_xor(__double2hiint(r), mask);
    int lo = __shfl_xor(__double2loint(r), mask);

    return __hiloint2double(hi, lo);
}

// Perform the equivalent of "__shfl_down" on an 8-byte value
static __device__ __forceinline__ double shfl_down(double r, int offset)
{
    int hi = __shfl_down(__double2hiint(r), offset);
    int lo = __shfl_down(__double2loint(r), offset);

    return __hiloint2double(hi, lo);
}

// Perform a reduction on a warp or the first part of it
template <unsigned int REDUCE_START_OFFSET>
__device__ __forceinline__ double warp_reduce(double r)
{
#pragma unroll
    for (int i = REDUCE_START_OFFSET; i >= 1; i >>= 1)
    {
        r += shfl_down(r, i);
    }

    return r;
}

// Perform 2 reductions, using either 1 or 2 warps
template <unsigned int REDUCE_START_OFFSET, bool HAVE_2_WARPS>
__device__ __forceinline__ void double_warp_reduce(double * dotp_s, int w_off)
{
    int t_idx = threadIdx.x;

    if (HAVE_2_WARPS)
    {
        // In this case, we have 2 warps, each doing 1 reduction
        if (t_idx < 64) 
        {
            dotp_s[w_off + t_idx] = warp_reduce<REDUCE_START_OFFSET>(dotp_s[w_off + t_idx] + dotp_s[w_off + t_idx + 32]);
        }
    }
    else
    {
        // In this case we have 1 warp that performs both reductions
        if (t_idx < 32)
        {
            dotp_s[t_idx] = warp_reduce<REDUCE_START_OFFSET>(dotp_s[t_idx] + dotp_s[t_idx + 32]);
            dotp_s[t_idx + 64] = warp_reduce<REDUCE_START_OFFSET>(dotp_s[t_idx + 64] + dotp_s[t_idx + 96]);
        }
    }    
}

// Synchronization wrapper, removing explicit synchronization when the thread-block is at most 32 threads (1 warp) in size 
template <bool MUST_SYNC>
__device__ __forceinline__ void sync_threads()
{
    if (MUST_SYNC)
    {
        __syncthreads();
    }
}

// Reset the entire contents of a shared reduction block; the thread block size must be a power-of-2
__device__ __forceinline__ void reset_dotp_buffers(double * const __restrict__ s_block)
{
    if (blockDim.x >= 64) 
    {
        int t_idx = threadIdx.x;

        if (t_idx < 64)
        {
            s_block[t_idx] = s_block[t_idx + 64] = 0.0;
        }
    }
    else    
    {
        int s_chunk = 128 / blockDim.x;
        int s_chunk_size = s_chunk * sizeof(double);

        // Each thread resets an equally-sized, contiguous portion of the buffer
        memset(s_block + threadIdx.x * s_chunk, 0, s_chunk_size);
    }
}

// =========================
// Backtransformation kernel
// =========================

// We use templates here to avoid additional branching based on the actual size of the thread-block
template<unsigned int REDUCE_START_OFFSET, bool HAVE_2_WARPS>
__global__ void __launch_bounds__(128) compute_hh_trafo_c_kernel(double * const __restrict__ q, const double * const __restrict__ hh, const double * const __restrict__ hh_dot,
    const double * const __restrict__ hh_tau, const int nb, const int ldq, const int off, const int ncols)
{
    __shared__ double dotp_s[128];
    __shared__ double q_s[129];

    int b_idx, t_idx, q_off, h_off, w_off, j, t_s, q_delta, hh_delta;
    double q_v_1, q_v_2, hh_v_1, hh_v_2, tau_1, tau_2, s_1, s_2, dot_p, hh_v_3, my_r1, my_r2;

    // The block index selects the eigenvector (EV) which the current block is responsible for
    b_idx = blockIdx.x;

    // The thread index selects the position inside the eigenvector selected above
    t_idx = threadIdx.x; 

    // The warp offset for the current thread: 0 for the first warp, 32 for the second etc.
    w_off = (t_idx >> 5) << 5;

    // The entire contents of the shared reduction buffers must be reset
    reset_dotp_buffers(dotp_s);

    // Compute initial access indices
    j = off + ncols - 1;
    q_off = b_idx + (j + t_idx) * ldq;
    h_off = j * nb + t_idx;
    t_s = t_idx >> 1;
    q_delta = ldq << 1;
    hh_delta = nb << 1;

    // Load the last EV components in the EV cache
    if (t_idx > 0)
    {
        q_s[t_idx + 1] = q[q_off];
    }

    // Ensure the ring buffer and reduction buffers are initialized
    sync_threads<HAVE_2_WARPS>();

    while (j >= off + 1)
    {
        // Per-iteration GMem I/O reads are in order to improve cache hit ratio

        // Read the corresponding compotents in the 2 Householder reflectors
        hh_v_1 = __ldg(&hh[h_off]);
        hh_v_2 = __ldg(&hh[h_off - nb]);
        hh_v_3 = (t_idx == 0)? 0.0 : __ldg(&hh[h_off - 1]);

        // Read the pre-computed dot-product of the 2 Householder reflectors
        dot_p = __ldg(&hh_dot[j - 1]);

        // Read the pre-computed values for "Tau" corresponding to the 2 Householder reflectors
        tau_1 = __ldg(&hh_tau[j]);
        tau_2 = __ldg(&hh_tau[j - 1]);

        // Only read the new EV components (the others are already stored in the shared EV cache, q_s)
        if (t_idx == 0)
        {
            q_s[0] = q[q_off - ldq];
            q_s[1] = q[q_off];
        }

        // Fill the shared buffers for the dot products bewtween the EV subset and the Householder reflectors         
        q_v_1 = q_s[t_idx + 1];
        q_v_2 = q_s[t_idx];

        my_r1 = q_v_1 * hh_v_1 * tau_1;
        my_r2 = q_v_2 * hh_v_2 * tau_2;

        // After using "shfl_xor", both threads in a pair will hold the same values
        my_r1 += shfl_xor(my_r1, 1);
        my_r2 += shfl_xor(my_r2, 1);
        
        // Now both threads in a pair can write to the same reduction buffer address without race-condition issues
        dotp_s[t_s] = my_r1;
        dotp_s[t_s + 64] = my_r2;

        // Ensure the reduction buffers are fully populated
        sync_threads<HAVE_2_WARPS>();

        // Perform the 2 reductions using only the first warp (we assume the warp size is 32, valid up to CC 3.x)
        double_warp_reduce<REDUCE_START_OFFSET, HAVE_2_WARPS>(dotp_s, w_off);
        
        // Ensure every thread will have access to the reduction results
        sync_threads<HAVE_2_WARPS>();

        // Each thread collects the reduction results
        s_1 = dotp_s[0]; 
        s_2 = dotp_s[64];

        // Each thread updates its corresponding EV component
        q_v_2 = q_v_2 - hh_v_3 * s_1 - hh_v_2 * s_2 + tau_2 * hh_v_2 * s_1 * dot_p;  

        if (t_idx == blockDim.x - 1)
        {
            // The last thread writes the last 2 EV components to the EV matrix
            q[q_off] = q_v_1 - hh_v_1 * s_1;
            q[q_off - ldq] = q_v_2;
        }
        else
        {
            // All other threads update the EV cache for the next iteration
            q_s[t_idx + 2] = q_v_2;
        }
        
        sync_threads<HAVE_2_WARPS>();

        // Update access indices
        q_off -= q_delta;
        h_off -= hh_delta;
        j -= 2;
    }

    // Once the previous loop has finished, we have at most 1 more iteration to perform

    if (j == off - 1)
    {
        // No iterations remain, so the final contents of the EV matrix are updated
        if (t_idx < blockDim.x - 1)
        {
            q[q_off + ldq] = q_v_2;
        }
    }
    else
    {
        // One iteration remains; it must be processed separately
        if (t_idx == 0)
        {
            // Only one more EV element needs to be loaded
            q_s[1] = q[q_off];
        }
            
        // As before, we first read the EV and Householder components
        q_v_1 = q_s[t_idx + 1];
        hh_v_1 = __ldg(&hh[h_off]);
        tau_1 = __ldg(&hh_tau[j]);

        // We prepare the reduction buffer
        my_r1 = q_v_1 * hh_v_1 * tau_1;
        my_r1 += shfl_xor(my_r1, 1);
        dotp_s[t_s] = my_r1;

        sync_threads<HAVE_2_WARPS>();

        // We perform the reduction using the first warp only
        if (t_idx < 32)
        {
            dotp_s[t_idx] = warp_reduce<REDUCE_START_OFFSET>(dotp_s[t_idx] + dotp_s[t_idx + 32]);
        }

        sync_threads<HAVE_2_WARPS>();
        
        // The last EV components are written to the EV matrix
        q[q_off] = q_v_1 - hh_v_1 * dotp_s[0];
    }
}

// This is a host wrapper for calling the appropriate back-transformation kernel, based on the SCALAPACK block size
extern "C" void launch_compute_hh_trafo_c_kernel(double * const q, const double * const hh, const double * const hh_dot,
        const double * const hh_tau, const int nev, const int nb, const int ldq, const int off, const int ncols)
{
    switch (nb)
    {
        case 128:
        case 64:
            compute_hh_trafo_c_kernel<16, true><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        case 32:
            compute_hh_trafo_c_kernel<8, false><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        case 16:
            compute_hh_trafo_c_kernel<4, false><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        case 8:
            compute_hh_trafo_c_kernel<2, false><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        case 4:
            compute_hh_trafo_c_kernel<1, false><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        case 2:
        case 1:
            compute_hh_trafo_c_kernel<0, false><<<nev, nb>>>(q, hh, hh_dot, hh_tau, nb, ldq, off, ncols);
            break;

        default:
            printf("Error: please use a power-of-2 SCALAPACK block size which is between 1 and 128.\n");
    }
}
