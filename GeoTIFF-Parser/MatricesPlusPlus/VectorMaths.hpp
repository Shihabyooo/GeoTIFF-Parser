//TODO consider adding a _BEST_PERF flag that simply uses the highest IS enabled by the compiler (i.e. wrap the #ifdef block in #ifndef _BEST_PERF,
// and in the #else section test def pf __AVX512F__ et al and redefine the flag accordinegly).

//TODO test the benefit of having a function specially for scalar multiplication. Would be similar to normal multiplication, except using loaders
//of style _m256_set1_ps() to load the scalar vector.

#pragma once
#include <immintrin.h>

//Flags to set instruction set to be used in vectorized arithmatics. Only max set (SSE, AVX256 or AVX512) would be used.
//#define _USE_SSE //Allows use of 128bit SIMD intrinsics
//#define _USE_AVX256 //ALlows use of 256bit SIMD intrinsics
//#define _USE_AVX512 //Allows use of 512bit SIMD intrinsics //AVX-512 is not common in mainstream processors

#define _USE_FMA256 //Allows use of 256bit fused mulitply-add intrinsics.

//Set size of vectors used in SIMD functions based on
//TODO replace this _BEST_PERF impl above
#ifdef _USE_AVX512
    #ifndef __AVX512F__
        #error Attempting to compile for 512bit AVX instructions without AVX-512F support.
    #endif
    #define _VECTOR_SIZE_F32 16
	#define _VECTOR_SIZE_F64 8
    #define _VECTORIZED_CODE
#elif defined(_USE_AVX256)
    #ifndef __AVX2__
        #error Attempting to compile for 256bit AVX instructions without AVX2 support.
    #endif
    #define _VECTOR_SIZE_F32 8
	#define _VECTOR_SIZE_F64 4
    #define _VECTORIZED_CODE
#elif defined(_USE_SSE)
    #ifndef __SSE2__
        #error Attempting to compile for 128bit SSE instructions without SSE2 support.
    #endif
    #define _VECTOR_SIZE_F32 4
	#define _VECTOR_SIZE_F64 2
    #define _VECTORIZED_CODE
#endif


#ifdef _USE_SSE
    void AddVectors128_f32(const float * vecA, const float * vecB, float * output); //output = vecA + vecB, each vector must consist of 4 floats (single precision)
    void AddVectors128_f64(const double * vecA, const double * vecB, double * output); //output = vecA + vecB, each vector must consist of 2 doubles (double precision)
    //void AddVectors128_i32(const int * vecA, const int * vecB, int * output); //output = vecA + vecB, each vector must consist of 4 ints (32bit integers)
    //void AddVectors128_i64(const long * vecA, const long * vecB, long * output); //output = vecA + vecB, each vector must consist of 2 longs (64bit integers)

    void SubtractVectors128_f32(const float * vecA, const float * vecB, float * output); //output = vecA - vecB, each vector must consist of 4 floats (single precision)
    void SubtractVectors128_f64(const double * vecA, const double * vecB, double * output); //output = vecA - vecB, each vector must consist of 2 doubles (double precision)
    //void SubtractVectors128_i32(const int * vecA, const int * vecB, int * output); //output = vecA - vecB, each vector must consist of 4 ints (32bit integers)
    //void SubtractVectors128_i64(const long * vecA, const long * vecB, long * output); //output = vecA - vecB, each vector must consist of 2 longs (64bit integers)

    void MultiplyVectors128_f32(const float * vecA, const float * vecB, float * output); //output = vecA * vecB, each vector must consist of 4 floats (single precision)
    void MultiplyVectors128_f64(const double * vecA, const double * vecB, double * output); //output = vecA * vecB, each vector must consist of 2 doubles (double precision)
    //void MultiplyVectors128_i32(const int * vecA, const int * vecB, int * output); //output = vecA * vecB, each vector must consist of 4 ints (32bit integers)
    //void MultiplyVectors128_i64(const long * vecA, const long * vecB, long * output); //output = vecA * vecB, each vector must consist of 2 longs (64bit integers)
#endif

#ifdef _USE_AVX256
    void AddVectors256_f32(const float * vecA, const float * vecB, float * output); //output = vecA + vecB, each vector must consist of 8 floats (single precision)
    void AddVectors256_f64(const double * vecA, const double * vecB, double * output); //output = vecA + vecB, each vector must consist of 4 doubles (double precision)
    //void AddVectors256_i32(const int * vecA, const int * vecB, int * output); //output = vecA + vecB, each vector must consist of 8 ints (32bit integers)
    //void AddVectors256_i64(const long * vecA, const long * vecB, long * output); //output = vecA + vecB, each vector must consist of 4 longs (64bit integers)

    void SubtractVectors256_f32(const float * vecA, const float * vecB, float * output); //output = vecA - vecB, each vector must consist of 8 floats (single precision)
    void SubtractVectors256_f64(const double * vecA, const double * vecB, double * output); //output = vecA - vecB, each vector must consist of 4 doubles (double precision)
    //void SubtractVectors256_i32(const int * vecA, const int * vecB, int * output); //output = vecA - vecB, each vector must consist of 8 ints (32bit integers)
    //void SubtractVectors256_i64(const long * vecA, const long * vecB, long * output); //output = vecA - vecB, each vector must consist of 4 longs (64bit integers)
    
    void MultiplyVectors256_f32(const float * vecA, const float * vecB, float * output); //output = vecA * vecB, each vector must consist of 8 floats (single precision)
    void MultiplyVectors256_f64(const double * vecA, const double * vecB, double * output); //output = vecA * vecB, each vector must consist of 4 doubles (double precision)
    //void MultiplyVectors256_i32(const int * vecA, const int * vecB, int * output); //output = vecA * vecB, each vector must consist of 8 ints (32bit integers)
    //void MultiplyVectors256_i64(const long * vecA, const long * vecB, long * output); //output = vecA * vecB, each vector must consist of 4 longs (64bit integers)
#endif

#ifdef _USE_AVX512
	void AddVectors512_f32(const float * vecA, const float * vecB, float * output); //output = vecA + vecB, each vector must consist of 16 floats (single precision)
    void AddVectors512_f64(const double * vecA, const double * vecB, double * output); //output = vecA + vecB, each vector must consist of 8 doubles (double precision)
    void AddVectors512_i32(const int * vecA, const int * vecB, int * output); //output = vecA + vecB, each vector must consist of 16 ints (32bit integers)
    void AddVectors512_i64(const long * vecA, const long * vecB, long * output); //output = vecA + vecB, each vector must consist of 8 longs (64bit integers)

    void SubtractVectors512_f32(const float * vecA, const float * vecB, float * output); //output = vecA - vecB, each vector must consist of 16 floats (single precision)
    void SubtractVectors512_f64(const double * vecA, const double * vecB, double * output); //output = vecA - vecB, each vector must consist of 8 doubles (double precision)
    void SubtractVectors512_i32(const int * vecA, const int * vecB, int * output); //output = vecA - vecB, each vector must consist of 16 ints (32bit integers)
    void SubtractVectors512_i64(const long * vecA, const long * vecB, long * output); //output = vecA - vecB, each vector must consist of 8 longs (64bit integers)

    void MultiplyVectors512_f32(const float * vecA, const float * vecB, float * output); //output = vecA * vecB, each vector must consist of 16 floats (single precision)
    void MultiplyVectors512_f64(const double * vecA, const double * vecB, double * output); //output = vecA * vecB, each vector must consist of 8 doubles (double precision)
    void MultiplyVectors512_i32(const int * vecA, const int * vecB, int * output); //output = vecA * vecB, each vector must consist of 16 ints (32bit integers)
    //void MultiplyVectors512_i64(const long * vecA, const long * vecB, long * output); //output = vecA * vecB, each vector must consist of 8 longs (64bit integers)
#endif

#ifdef _USE_FMA256
    void AddMultiplyVectors_f32(const float * vecA, const float * vecB, const float * vecC, float * output); // Computes (A + B) + C, each vector must consist of 8 floats (single precision)
    void AddMultiplyVectors_f64(const double * vecA, const double * vecB, const double * vecC, double * output); // Computes (A + B) + C, each vector must consist of 4 doubles (double precision)
    //void AddMultiplyVectors_i32(const int * vecA, const int * vecB, const int * vecC, int * output); // Computes (A + B) + C, each vector must consist of 8 ints (32bit integers)
    //void AddMultiplyVectors_i64(const long * vecA, const long * vecB, const long * vecC, long * output); // Computes (A + B) + C, each vector must consist of 4 longs (64bit integers)

    void SubtractMultiplyVectors_f32(const float * vecA, const float * vecB, const float * vecC, float * output); // Computes (A + B) - C, each vector must consist of 8 floats (single precision)
    void SubtractMultiplyVectors_f64(const double * vecA, const double * vecB, const double * vecC, double * output); // Computes (A + B) - C, each vector must consist of 4 doubles (double precision)
    //void SubtractMultiplyVectors_i32(const int * vecA, const int * vecB, const int * vecC, int * output); // Computes (A + B) - C, each vector must consist of 8 ints (32bit integers)
    //void SubtractMultiplyVectors_i64(const long * vecA, const long * vecB, const long * vecC, long * output); // Computes (A + B) - C, each vector must consist of 4 longs (64bit integers)
#endif