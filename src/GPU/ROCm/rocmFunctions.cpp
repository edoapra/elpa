//
//    Copyright 2021, A. Marek
//
//    This file is part of ELPA.
//
//    The ELPA library was originally created by the ELPA consortium,
//    consisting of the following organizations:
//
//    - Max Planck Computing and Data Facility (MPCDF), formerly known as
//      Rechenzentrum Garching der Max-Planck-Gesellschaft (RZG),
//    - Bergische Universität Wuppertal, Lehrstuhl für angewandte
//      Informatik,
//    - Technische Universität München, Lehrstuhl für Informatik mit
//      Schwerpunkt Wissenschaftliches Rechnen ,
//    - Fritz-Haber-Institut, Berlin, Abt. Theorie,
//    - Max-Plack-Institut für Mathematik in den Naturwissenschaften,
//      Leipzig, Abt. Komplexe Strukutren in Biologie und Kognition,
//      and
//    - IBM Deutschland GmbH
//
//    This particular source code file contains additions, changes and
//    enhancements authored by Intel Corporation which is not part of
//    the ELPA consortium.
//
//    More information can be found here:
//    http://elpa.mpcdf.mpg.de/
//
//    ELPA is free software: you can redistribute it and/or modify
//    it under the terms of the version 3 of the license of the
//    GNU Lesser General Public License as published by the Free
//    Software Foundation.
//
//    ELPA is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with ELPA.  If not, see <http://www.gnu.org/licenses/>
//
//    ELPA reflects a substantial effort on the part of the original
//    ELPA consortium, and we ask you to respect the spirit of the
//    license that we chose: i.e., please contribute any changes you
//    may have back to the original ELPA library distribution, and keep
//    any derivatives of ELPA under the same license that we chose for
//    the original distribution, the GNU Lesser General Public License.
//
//
// --------------------------------------------------------------------------------------------------
//
// This file was written by A. Marek, MPCDF

#include <stdio.h>
#include <math.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <alloca.h>
#include <stdint.h>
#include <complex.h>

#include "config-f90.h"


#undef BLAS_status
#undef BLAS_handle
//#undef BLAS_float_complex
#undef BLAS_set_stream
#undef BLAS_status_success
#undef BLAS_status_invalid_handle
#undef BLAS_create_handle
#undef BLAS_destroy_handle
#undef BLAS_double_complex
#undef BLAS_float_complex
#undef BLAS_strsm
#undef BLAS_dtrsm
#undef BLAS_ctrsm
#undef BLAS_ztrsm
#undef BLAS_dtrmm
#undef BLAS_strmm
#undef BLAS_ztrmm
#undef BLAS_ctrmm
#undef BLAS_dcopy
#undef BLAS_scopy
#undef BLAS_zcopy
#undef BLAS_ccopy
#undef BLAS_dgemm
#undef BLAS_sgemm
#undef BLAS_zgemm
#undef BLAS_cgemm
#undef BLAS_dgemv
#undef BLAS_sgemv
#undef BLAS_zgemv
#undef BLAS_cgemv
#undef BLAS_operation
#undef BLAS_operation_none
#undef BLAS_operation_transpose
#undef BLAS_operation_conjugate_transpose
#undef BLAS_operation_none
#undef BLAS_fill
#undef BLAS_fill_lower
#undef BLAS_fill_upper
#undef BLAS_side
#undef BLAS_side_left
#undef BLAS_side_right
#undef BLAS_diagonal
#undef BLAS_diagonal_non_unit
#undef BLAS_diagonal_unit

#ifdef HIPBLAS
#define BLAS hipblas
#define BLAS_status hipblasStatus_t
#define BLAS_handle hipblasHandle_t
#define BLAS_set_stream hipblasSetStream
#define BLAS_status_success HIPBLAS_STATUS_SUCCESS
#define BLAS_status_invalid_handle HIPBLAS_STATUS_INVALID_VALUE
#define BLAS_create_handle hipblasCreate
#define BLAS_destroy_handle hipblasDestroy
#define BLAS_double_complex hipblasDoubleComplex
#define BLAS_float_complex hipblasComplex
#define BLAS_ctrsm hipblasCtrsm
#define BLAS_ztrsm hipblasZtrsm
#define BLAS_dtrsm hipblasDtrsm
#define BLAS_strsm hipblasStrsm
#define BLAS_ctrmm hipblasCtrmm
#define BLAS_ztrmm hipblasZtrmm
#define BLAS_dtrmm hipblasDtrmm
#define BLAS_strmm hipblasStrmm
#define BLAS_ccopy hipblasCcopy
#define BLAS_zcopy hipblasZcopy
#define BLAS_dcopy hipblasDcopy
#define BLAS_scopy hipblasScopy
#define BLAS_cgemm hipblasCgemm
#define BLAS_zgemm hipblasZgemm
#define BLAS_dgemm hipblasDgemm
#define BLAS_sgemm hipblasSgemm
#define BLAS_cgemv hipblasCgemv
#define BLAS_zgemv hipblasZgemv
#define BLAS_dgemv hipblasDgemv
#define BLAS_sgemv hipblasSgemv
#define BLAS_operation hipblasOperation_t
#define BLAS_operation_none HIPBLAS_OP_N
#define BLAS_operation_transpose HIPBLAS_OP_T
#define BLAS_operation_conjugate_transpose HIPBLAS_OP_C
#define BLAS_operation_none HIPBLAS_OP_N
#define BLAS_fill hipblasFillMode_t
#define BLAS_fill_lower HIPBLAS_FILL_MODE_LOWER
#define BLAS_fill_upper HIPBLAS_FILL_MODE_UPPER
#define BLAS_side hipblasSideMode_t
#define BLAS_side_left HIPBLAS_SIDE_LEFT
#define BLAS_side_right HIPBLAS_SIDE_RIGHT
#define BLAS_diagonal hipblasDiagType_t
#define BLAS_diagonal_non_unit HIPBLAS_DIAG_NON_UNIT
#define BLAS_diagonal_unit HIPBLAS_DIAG_UNIT
//#define BLAS_float_complex hipblas_float_complex
//#define BLAS_set_stream hipblas_set_stream
#else /* HIPBLAS */
#define BLAS rocblas
#define BLAS_status rocblas_status
#define BLAS_handle rocblas_handle
#define BLAS_set_stream rocblas_set_stream
#define BLAS_status_success rocblas_status_success
#define BLAS_status_invalid_handle rocblas_status_invalid_handle
#define BLAS_status_memory_error rocblas_status_memory_error
#define BLAS_create_handle rocblas_create_handle
#define BLAS_destroy_handle rocblas_destroy_handle
#define BLAS_double_complex rocblas_double_complex
#define BLAS_float_complex rocblas_float_complex
#define BLAS_ctrsm rocblas_ctrsm
#define BLAS_ztrsm rocblas_ztrsm
#define BLAS_dtrsm rocblas_dtrsm
#define BLAS_strsm rocblas_strsm
#define BLAS_ctrmm rocblas_ctrmm
#define BLAS_ztrmm rocblas_ztrmm
#define BLAS_dtrmm rocblas_dtrmm
#define BLAS_strmm rocblas_strmm
#define BLAS_ccopy rocblas_ccopy
#define BLAS_zcopy rocblas_zcopy
#define BLAS_dcopy rocblas_dcopy
#define BLAS_scopy rocblas_scopy
#define BLAS_cgemm rocblas_cgemm
#define BLAS_zgemm rocblas_zgemm
#define BLAS_dgemm rocblas_dgemm
#define BLAS_sgemm rocblas_sgemm
#define BLAS_cgemv rocblas_cgemv
#define BLAS_zgemv rocblas_zgemv
#define BLAS_dgemv rocblas_dgemv
#define BLAS_sgemv rocblas_sgemv
#define BLAS_operation rocblas_operation
#define BLAS_operation_none rocblas_operation_none
#define BLAS_operation_transpose rocblas_operation_transpose
#define BLAS_operation_conjugate_transpose rocblas_operation_conjugate_transpose
#define BLAS_operation_none rocblas_operation_none
#define BLAS_fill rocblas_fill
#define BLAS_fill_lower rocblas_fill_lower
#define BLAS_fill_upper rocblas_fill_upper
#define BLAS_side rocblas_side
#define BLAS_side_left rocblas_side_left
#define BLAS_side_right rocblas_side_right
#define BLAS_diagonal rocblas_diagonal
#define BLAS_diagonal_non_unit rocblas_diagonal_non_unit
#define BLAS_diagonal_unit rocblas_diagonal_unit
#endif /* HIPBLAS */

#ifdef HIPBLAS
#include "hipblas.h"
#else
#include "rocblas.h"
#endif
#include "hip/hip_runtime_api.h"


#define errormessage(x, ...) do { fprintf(stderr, "%s:%d " x, __FILE__, __LINE__, __VA_ARGS__ ); } while (0)

#ifdef DEBUG_HIP
#define debugmessage(x, ...) do { fprintf(stderr, "%s:%d " x, __FILE__, __LINE__, __VA_ARGS__ ); } while (0)
#else
#define debugmessage(x, ...)
#endif

// hipStream_t elpa_hip_stm;

#ifdef WITH_AMD_GPU_VERSION
extern "C" {
  int hipStreamCreateFromC(intptr_t *stream) {
    *stream = (intptr_t) malloc(sizeof(hipStream_t));
    hipError_t status = hipStreamCreate((hipStream_t*) *stream);
    if (status == hipSuccess) {
//       printf("all OK\n");
      return 1;
    }
    else{
      errormessage("Error in hipStreamCreate: %s\n", "unknown error");
      return 0;
    }

  }

  int hipStreamDestroyFromC(intptr_t *stream){
    hipError_t status = hipStreamDestroy(*(hipStream_t*) *stream);
    *stream = (intptr_t) NULL;
    if (status ==hipSuccess) {
//       printf("all OK\n");
      return 1;
    }
    else{
      errormessage("Error in hipStreamDestroy: %s\n", "unknown error");
      return 0;
    }
  }

  int rocblasSetStreamFromC(intptr_t handle, intptr_t stream) {
    BLAS_status status = BLAS_set_stream(*((BLAS_handle*)handle), *((hipStream_t*)stream));
    if (status == BLAS_status_success ) {
      return 1;
    }
    else if (status == BLAS_status_invalid_handle) {
      errormessage("Error in rocblasSetStream: %s\n", "the HIP Runtime initialization failed");
      return 0;
    }
    else{
      errormessage("Error in rocblasSetStream: %s\n", "unknown error");
      return 0;
    }
  }

  int hipStreamSynchronizeExplicitFromC(intptr_t stream) {
    hipError_t status = hipStreamSynchronize(*((hipStream_t*)stream));
    if (status == hipSuccess) {
      return 1;
    }
    else{
      errormessage("Error in hipStreamSynchronizeExplicit: %s\n", "unknown error");
      return 0;
    }
  }

  int hipStreamSynchronizeImplicitFromC() {
    hipError_t status = hipStreamSynchronize(hipStreamPerThread);
    if (status == hipSuccess) {
      return 1;
    }
    else{
      errormessage("Error in hipStreamSynchronizeImplicit: %s\n", "unknown error");
      return 0;
    }
  }

  int hipMemcpy2dAsyncFromC(intptr_t *dest, size_t dpitch, intptr_t *src, size_t spitch, size_t width, size_t height, int dir, intptr_t stream) {

    hipError_t hiperr = hipMemcpy2DAsync( dest, dpitch, src, spitch, width, height, (hipMemcpyKind)dir, *((hipStream_t*)stream) );
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemcpy2dAsync: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }


  int rocblasCreateFromC(intptr_t *handle) {
//     printf("in c: %p\n", *cublas_handle);
    *handle = (intptr_t) malloc(sizeof(BLAS_handle));
//     printf("in c: %p\n", *cublas_handle);
    BLAS_status status = BLAS_create_handle((BLAS_handle*) *handle);
    if (status == BLAS_status_success) {
//       printf("all OK\n");
      return 1;
    }
    else if (status == BLAS_status_invalid_handle) {
      errormessage("Error in rocblas_create_handle: %s\n", "the rocblas Runtime initialization failed");
      return 0;
    }
#ifndef HIPBLAS
    else if (status == BLAS_status_memory_error) {
      errormessage("Error in rocblas_create_handle: %s\n", "the resources could not be allocated");
      return 0;
    }
#endif
    else{
      errormessage("Error in rocblas_create_handle: %s\n", "unknown error");
      return 0;
    }
#if 0
    if (hipStreamCreate(&elpa_hip_stm) != hipSuccess) {
        errormessage("failed to create stream, %s, %d\n", __FILE__, __LINE__);
        return 0;
    }

    if (rocblas_set_stream(*(rocblas_handle *)handle, elpa_hip_stm) != rocblas_status_success) {
        errormessage("failed to attach stream to blas handle, %s, %d\n", __FILE__, __LINE__);
        return EXIT_FAILURE;
    }
#endif
  }

  int rocblasDestroyFromC(intptr_t *handle) {
    BLAS_status status = BLAS_destroy_handle(*((BLAS_handle*) *handle));
    *handle = (intptr_t) NULL;
    if (status == BLAS_status_success) {
//       printf("all OK\n");
      return 1;
    }
    else if (status == BLAS_status_invalid_handle) {
      errormessage("Error in rocblas_destroy_handle: %s\n", "the library has not been initialized");
      return 0;
    }
    else{
      errormessage("Error in rocblas_destroy_handle: %s\n", "unknown error");
      return 0;
    }
#if 0
    hipStreamDestroy(elpa_hip_stm);
#endif
  }

  int hipSetDeviceFromC(int n) {

    hipError_t hiperr = hipSetDevice(n);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipSetDevice: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipGetDeviceCountFromC(int *count) {

    hipError_t hiperr = hipGetDeviceCount(count);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipGetDeviceCount: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipDeviceSynchronizeFromC() {

    hipError_t hiperr = hipDeviceSynchronize();
    if (hiperr != hipSuccess) {
      errormessage("Error in hipDeviceSynchronize: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMallocFromC(intptr_t *a, size_t width_height) {

    hipError_t hiperr = hipMalloc((void **) a, width_height);
#ifdef DEBUG_HIP
    printf("HIP Malloc,  pointer address: %p, size: %d \n", *a, width_height);
#endif
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMalloc: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipFreeFromC(intptr_t *a) {
#ifdef DEBUG_HIP
    printf("HIP Free, pointer address: %p \n", a);
#endif
    hipError_t hiperr = hipFree(a);

    if (hiperr != hipSuccess) {
      errormessage("Error in hipFree: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipHostMallocFromC(intptr_t *a, size_t width_height) {

    hipError_t hiperr = hipHostMalloc((void **) a, width_height, hipHostMallocMapped);
#ifdef DEBUG_HIP
    printf("MallocHost pointer address: %p \n", *a);
#endif
    if (hiperr != hipSuccess) {
      errormessage("Error in hipHostMalloc: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipHostFreeFromC(intptr_t *a) {
#ifdef DEBUG_HIP
    printf("FreeHost pointer address: %p \n", a);
#endif
    hipError_t hiperr = hipHostFree(a);

    if (hiperr != hipSuccess) {
      errormessage("Error in hipHostFree: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemsetFromC(intptr_t *a, int value, size_t count) {

    hipError_t hiperr = hipMemset( a, value, count);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemset: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemsetAsyncFromC(intptr_t *a, int value, size_t count, intptr_t stream) {

    hipError_t hiperr = hipMemsetAsync( a, value, count, *((hipStream_t*)stream));
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemsetAsync: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemcpyFromC(intptr_t *dest, intptr_t *src, size_t count, int dir) {

    hipError_t hiperr = hipMemcpy( dest, src, count, (hipMemcpyKind)dir);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemcpy: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemcpyAsyncFromC(intptr_t *dest, intptr_t *src, size_t count, int dir, intptr_t stream) {
  
    hipError_t hiperr = hipMemcpyAsync( dest, src, count, (hipMemcpyKind)dir, *((hipStream_t*)stream));
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemcpyAsync: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemcpy2dFromC(intptr_t *dest, size_t dpitch, intptr_t *src, size_t spitch, size_t width, size_t height, int dir) {

    hipError_t hiperr = hipMemcpy2D( dest, dpitch, src, spitch, width, height, (hipMemcpyKind)dir);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipMemcpy2d: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipHostRegisterFromC(intptr_t *a, intptr_t value, int flag) {

    hipError_t hiperr = hipHostRegister( a, value, (unsigned int)flag);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipHostRegister: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipHostUnregisterFromC(intptr_t *a) {

    hipError_t hiperr = hipHostUnregister( a);
    if (hiperr != hipSuccess) {
      errormessage("Error in hipHostUnregister: %s\n",hipGetErrorString(hiperr));
      return 0;
    }
    return 1;
  }

  int hipMemcpyDeviceToDeviceFromC(void) {
      int val = (int)hipMemcpyDeviceToDevice;
      return val;
  }
  int hipMemcpyHostToDeviceFromC(void) {
      int val = (int)hipMemcpyHostToDevice;
      return val;
  }
  int hipMemcpyDeviceToHostFromC(void) {
      int val = (int)hipMemcpyDeviceToHost;
      return val;
  }
  int hipHostRegisterDefaultFromC(void) {
      int val = (int)hipHostRegisterDefault;
      return val;
  }
  int hipHostRegisterPortableFromC(void) {
      int val = (int)hipHostRegisterPortable;
      return val;
  }
  int hipHostRegisterMappedFromC(void) {
      int val = (int)hipHostRegisterMapped;
      return val;
  }

  BLAS_operation hip_operation(char trans) {
    if (trans == 'N' || trans == 'n') {
      return BLAS_operation_none;
    }
    else if (trans == 'T' || trans == 't') {
      return BLAS_operation_transpose;
    }
    else if (trans == 'C' || trans == 'c') {
      return BLAS_operation_conjugate_transpose;
    }
    else {
      errormessage("Error when transfering %c to rocblas_Operation_t\n",trans);
      // or abort?
      return BLAS_operation_none;
    }
  }


  BLAS_fill hip_fill_mode(char uplo) {
    if (uplo == 'L' || uplo == 'l') {
      return BLAS_fill_lower;
    }
    else if(uplo == 'U' || uplo == 'u') {
      return BLAS_fill_upper;
    }
    else {
      errormessage("Error when transfering %c to cublasFillMode_t\n", uplo);
      // or abort?
      return BLAS_fill_lower;
    }
  }

  BLAS_side hip_side_mode(char side) {
    if (side == 'L' || side == 'l') {
      return BLAS_side_left;
    }
    else if (side == 'R' || side == 'r') {
      return BLAS_side_right;
    }
    else{
      errormessage("Error when transfering %c to rocblas_side\n", side);
      // or abort?
      return BLAS_side_left;
    }
  }

  BLAS_diagonal hip_diag_type(char diag) {
    if (diag == 'N' || diag == 'n') {
      return BLAS_diagonal_non_unit;
    }
    else if (diag == 'U' || diag == 'u') {
      return BLAS_diagonal_unit;
    }
    else {
      errormessage("Error when transfering %c to rocblas_diag\n", diag);
      // or abort?
      return BLAS_diagonal_non_unit;
    }
  }


  void rocblas_dgemv_elpa_wrapper (intptr_t handle, char trans, int m, int n, double alpha,
                               const double *A, int lda,  const double *x, int incx,
                               double beta, double *y, int incy) {

    BLAS_status status = BLAS_dgemv(*((BLAS_handle*)handle), hip_operation(trans),
                m, n, &alpha, A, lda, x, incx, &beta, y, incy);
  }

  void rocblas_sgemv_elpa_wrapper (intptr_t handle, char trans, int m, int n, float alpha,
                               const float *A, int lda,  const float *x, int incx,
                               float beta, float *y, int incy) {

    BLAS_status status = BLAS_sgemv(*((BLAS_handle*)handle), hip_operation(trans),
                m, n, &alpha, A, lda, x, incx, &beta, y, incy);
  }

  void rocblas_zgemv_elpa_wrapper (intptr_t handle, char trans, int m, int n, double _Complex alpha,
                               const double _Complex *A, int lda,  const double _Complex *x, int incx,
                               double _Complex beta, double _Complex *y, int incy) {

    BLAS_double_complex alpha_casted = *((BLAS_double_complex*)(&alpha));
    BLAS_double_complex beta_casted = *((BLAS_double_complex*)(&beta));

#ifndef HIPBLAS
    const BLAS_double_complex* A_casted = (const BLAS_double_complex*) A;
    const BLAS_double_complex* x_casted = (const BLAS_double_complex*) x;
#else
    BLAS_double_complex* A_casted = (BLAS_double_complex*) A;
    BLAS_double_complex* x_casted = (BLAS_double_complex*) x;
#endif
    BLAS_double_complex* y_casted = (BLAS_double_complex*) y;

    BLAS_status status = BLAS_zgemv(*((BLAS_handle*)handle), hip_operation(trans),
                m, n, &alpha_casted, A_casted, lda, x_casted, incx, &beta_casted, y_casted, incy);
  }

  void rocblas_cgemv_elpa_wrapper (intptr_t handle, char trans, int m, int n, float _Complex alpha,
                               const float _Complex *A, int lda,  const float _Complex *x, int incx,
                               float _Complex beta, float _Complex *y, int incy) {

    BLAS_float_complex alpha_casted = *((BLAS_float_complex*)(&alpha));
    BLAS_float_complex beta_casted = *((BLAS_float_complex*)(&beta));

#ifndef HIPBLAS
    const BLAS_float_complex* A_casted = (const BLAS_float_complex*) A;
    const BLAS_float_complex* x_casted = (const BLAS_float_complex*) x;
#else
          BLAS_float_complex* A_casted = (      BLAS_float_complex*) A;
          BLAS_float_complex* x_casted = (      BLAS_float_complex*) x;
#endif
    BLAS_float_complex* y_casted = (BLAS_float_complex*) y;

    BLAS_status status = BLAS_cgemv(*((BLAS_handle*)handle), hip_operation(trans),
                m, n, &alpha_casted, A_casted, lda, x_casted, incx, &beta_casted, y_casted, incy);
  }


  void rocblas_dgemm_elpa_wrapper (intptr_t handle, char transa, char transb, int m, int n, int k,
                               double alpha, const double *A, int lda,
                               const double *B, int ldb, double beta,
                               double *C, int ldc) {

    BLAS_status status = BLAS_dgemm(*((BLAS_handle*)handle), hip_operation(transa), hip_operation(transb),
                m, n, k, &alpha, A, lda, B, ldb, &beta, C, ldc);
  }

  void rocblas_sgemm_elpa_wrapper (intptr_t handle, char transa, char transb, int m, int n, int k,
                               float alpha, const float *A, int lda,
                               const float *B, int ldb, float beta,
                               float *C, int ldc) {

    BLAS_status status = BLAS_sgemm(*((BLAS_handle*)handle), hip_operation(transa), hip_operation(transb),
                m, n, k, &alpha, A, lda, B, ldb, &beta, C, ldc);
  }

  void rocblas_zgemm_elpa_wrapper (intptr_t handle, char transa, char transb, int m, int n, int k,
                               double _Complex alpha, const double _Complex *A, int lda,
                               const double _Complex *B, int ldb, double _Complex beta,
                               double _Complex *C, int ldc) {

    BLAS_double_complex alpha_casted = *((BLAS_double_complex*)(&alpha));
    BLAS_double_complex beta_casted = *((BLAS_double_complex*)(&beta));

#ifndef HIPBLAS
    const BLAS_double_complex* A_casted = (const BLAS_double_complex*) A;
    const BLAS_double_complex* B_casted = (const BLAS_double_complex*) B;
#else
          BLAS_double_complex* A_casted = (      BLAS_double_complex*) A;
          BLAS_double_complex* B_casted = (      BLAS_double_complex*) B;
#endif
    BLAS_double_complex* C_casted = (BLAS_double_complex*) C;

    BLAS_status status = BLAS_zgemm(*((BLAS_handle*)handle), hip_operation(transa), hip_operation(transb),
                m, n, k, &alpha_casted, A_casted, lda, B_casted, ldb, &beta_casted, C_casted, ldc);
  }

  void rocblas_cgemm_elpa_wrapper (intptr_t handle, char transa, char transb, int m, int n, int k,
                               float _Complex alpha, const float _Complex *A, int lda,
                               const float _Complex *B, int ldb, float _Complex beta,
                               float _Complex *C, int ldc) {

    BLAS_float_complex alpha_casted = *((BLAS_float_complex*)(&alpha));
    BLAS_float_complex beta_casted = *((BLAS_float_complex*)(&beta));

#ifndef HIPBLAS
    const BLAS_float_complex* A_casted = (const BLAS_float_complex*) A;
    const BLAS_float_complex* B_casted = (const BLAS_float_complex*) B;
#else
          BLAS_float_complex* A_casted = (      BLAS_float_complex*) A;
          BLAS_float_complex* B_casted = (      BLAS_float_complex*) B;
#endif
    BLAS_float_complex* C_casted = (BLAS_float_complex*) C;

    BLAS_status status = BLAS_cgemm(*((BLAS_handle*)handle), hip_operation(transa), hip_operation(transb),
                m, n, k, &alpha_casted, A_casted, lda, B_casted, ldb, &beta_casted, C_casted, ldc);
  }


  // todo: new CUBLAS API diverged from standard BLAS api for these functions
  // todo: it provides out-of-place (and apparently more efficient) implementation
  // todo: by passing B twice (in place of C as well), we should fall back to in-place algorithm


  void rocblasDcopy_elpa_wrapper (intptr_t handle, int n, double *x, int incx, double *y, int incy){

    BLAS_status status = BLAS_dcopy(*((BLAS_handle*)handle), n, x, incx, y, incy);
  }

  void rocblasScopy_elpa_wrapper (intptr_t handle, int n, float *x, int incx, float *y, int incy){

    BLAS_status status = BLAS_scopy(*((BLAS_handle*)handle), n, x, incx, y, incy);
  }

  void rocblasZcopy_elpa_wrapper (intptr_t handle, int n, double _Complex *x, int incx, double _Complex *y, int incy){
#ifndef HIPBLAS
    const BLAS_double_complex* X_casted = (const BLAS_double_complex*) x;
#else
          BLAS_double_complex* X_casted = (      BLAS_double_complex*) x;
#endif
          BLAS_double_complex* Y_casted = (BLAS_double_complex*) y;

    BLAS_status status = BLAS_zcopy(*((BLAS_handle*)handle), n, X_casted, incx, Y_casted, incy);
  }

  void rocblasCcopy_elpa_wrapper (intptr_t handle, int n, float _Complex *x, int incx, float _Complex *y, int incy){
#ifndef HIPBLAS
    const BLAS_float_complex* X_casted = (const BLAS_float_complex*) x;
#else
          BLAS_float_complex* X_casted = (      BLAS_float_complex*) x;
#endif
          BLAS_float_complex* Y_casted = (      BLAS_float_complex*) y;

    BLAS_status status = BLAS_ccopy(*((BLAS_handle*)handle), n, X_casted, incx, Y_casted, incy);
  }


  void rocblas_dtrmm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, double alpha, const double *A,
                               int lda, double *B, int ldb){

    BLAS_status status = BLAS_dtrmm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha, A, lda, B, ldb);
  }

  void rocblas_strmm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, float alpha, const float *A,
                               int lda, float *B, int ldb){
    BLAS_status status = BLAS_strmm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha, A, lda, B, ldb);
  }

  void rocblas_ztrmm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, double _Complex alpha, const double _Complex *A,
                               int lda, double _Complex *B, int ldb){

    BLAS_double_complex alpha_casted = *((BLAS_double_complex*)(&alpha));

#ifndef HIPBLAS
    const BLAS_double_complex* A_casted = (const BLAS_double_complex*) A;
#else
          BLAS_double_complex* A_casted = (      BLAS_double_complex*) A;
#endif
    BLAS_double_complex* B_casted = (BLAS_double_complex*) B;
    BLAS_status status = BLAS_ztrmm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha_casted, A_casted, lda, B_casted, ldb);
  }

  void rocblas_ctrmm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, float _Complex alpha, const float _Complex *A,
                               int lda, float _Complex *B, int ldb){

    BLAS_float_complex alpha_casted = *((BLAS_float_complex*)(&alpha));

#ifndef HIPBLAS
    const BLAS_float_complex* A_casted = (const BLAS_float_complex*) A;
#else
          BLAS_float_complex* A_casted = (      BLAS_float_complex*) A;
#endif
    BLAS_float_complex* B_casted = (BLAS_float_complex*) B;
    BLAS_status status = BLAS_ctrmm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha_casted, A_casted, lda, B_casted, ldb);
  }


  void rocblas_dtrsm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, double alpha, double *A,
                               int lda, double *B, int ldb){

    BLAS_status status = BLAS_dtrsm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha, A, lda, B, ldb);
  }

  void rocblas_strsm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, float alpha, float *A,
                               int lda, float *B, int ldb){
    BLAS_status status = BLAS_strsm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha, A, lda, B, ldb);
  }

  void rocblas_ztrsm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, double _Complex alpha, const double _Complex *A,
                               int lda, double _Complex *B, int ldb){

    BLAS_double_complex alpha_casted = *((BLAS_double_complex*)(&alpha));

#ifndef HIPBLAS
    const BLAS_double_complex* A_casted = (const BLAS_double_complex*) A;
#else
          BLAS_double_complex* A_casted = (      BLAS_double_complex*) A;
#endif
    BLAS_double_complex* B_casted = (BLAS_double_complex*) B;
    BLAS_status status = BLAS_ztrsm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha_casted, A_casted, lda, B_casted, ldb);
  }

  void rocblas_ctrsm_elpa_wrapper (intptr_t handle, char side, char uplo, char transa, char diag,
                               int m, int n, float _Complex alpha, const float _Complex *A,
                               int lda, float _Complex *B, int ldb){

    BLAS_float_complex alpha_casted = *((BLAS_float_complex*)(&alpha));

#ifndef HIPBLAS
    const BLAS_float_complex* A_casted = (const BLAS_float_complex*) A;
#else
          BLAS_float_complex* A_casted = (      BLAS_float_complex*) A;
#endif
    BLAS_float_complex* B_casted = (BLAS_float_complex*) B;
    BLAS_status status = BLAS_ctrsm(*((BLAS_handle*)handle), hip_side_mode(side), hip_fill_mode(uplo), hip_operation(transa),
                hip_diag_type(diag), m, n, &alpha_casted, A_casted, lda, B_casted, ldb);
  }


}
#endif /* WITH_AMD_GPU_VERSION */
