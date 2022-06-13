#include "VectorMaths.hpp"

#ifdef _USE_SSE
void AddVectors128_f32(const float * vecA, const float * vecB, float * output)
{
    __m128 result = _mm_add_ps(_mm_loadu_ps(vecA), _mm_loadu_ps(vecB));
	_mm_storeu_ps(output, result);
}

void AddVectors128_f64(const double * vecA, const double * vecB, double * output)
{
    __m128d result = _mm_add_pd(_mm_loadu_pd(vecA), _mm_loadu_pd(vecB));
	_mm_storeu_pd(output, result);
}

// void AddVectors128_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m128i result = _mm_add_epi32(_mm_loadu_epi32(vecA), _mm_loadu_epi32(vecB));
// 	_mm_storeu_epi32(output, result);
// }

// void AddVectors128_i64(const long * vecA, const long * vecB, long * output)
// {   
//     __m128i result = _mm_add_epi64(_mm_loadu_epi64(vecA), _mm_loadu_epi64(vecB));
// 	_mm_storeu_epi64(output, result);
// }

void SubtractVectors128_f32(const float * vecA, const float * vecB, float * output)
{
    __m128 result = _mm_sub_ps(_mm_loadu_ps(vecA), _mm_loadu_ps(vecB));
	_mm_storeu_ps(output, result);
}

void SubtractVectors128_f64(const double * vecA, const double * vecB, double * output)
{
    __m128d result = _mm_sub_pd(_mm_loadu_pd(vecA), _mm_loadu_pd(vecB));
	_mm_storeu_pd(output, result);
}

// void SubtractVectors128_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m128i result = _mm_sub_epi32(_mm_loadu_epi32(vecA), _mm_loadu_epi32(vecB));
// 	_mm_storeu_epi32(output, result);
// }

// void SubtractVectors128_i64(const long * vecA, const long * vecB, long * output)
// {
//     __m128i result = _mm_sub_epi64(_mm_loadu_epi64(vecA), _mm_loadu_epi64(vecB));
// 	_mm_storeu_epi64(output, result);
// }

void MultiplyVectors128_f32(const float * vecA, const float * vecB, float * output)
{
    __m128 result = _mm_mul_ps(_mm_loadu_ps(vecA), _mm_loadu_ps(vecB));
	_mm_storeu_ps(output, result);
}

// void MultiplyVectors128_f64(const double * vecA, const double * vecB, double * output)
// {
//     __m128d result = _mm_mul_pd(_mm_loadu_pd(vecA), _mm_loadu_pd(vecB));
// 	_mm_storeu_pd(output, result);
// }

// void MultiplyVectors128_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m128i result = _mm_mul_epi32(_mm_loadu_epi32(vecA), _mm_loadu_epi32(vecB));
// 	_mm_storeu_epi32(output, result);
// }

// void MultiplyVectors128_i64(const long * vecA, const long * vecB, long * output)
// {
// }
#endif

#ifdef _USE_AVX256
void AddVectors256_f32(const float * vecA, const float * vecB, float * output)
{
    __m256 result = _mm256_add_ps(_mm256_loadu_ps(vecA), _mm256_loadu_ps(vecB));
	_mm256_storeu_ps(output, result);
}

void AddVectors256_f64(const double * vecA, const double * vecB, double * output)
{
    __m256d result = _mm256_add_pd(_mm256_loadu_pd(vecA), _mm256_loadu_pd(vecB));
	_mm256_storeu_pd(output, result);
}

// void AddVectors256_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m256i result = _mm256_add_epi32(_mm256_loadu_epi32(vecA), _mm256_loadu_epi32(vecB));
// 	_mm256_storeu_epi32(output, result);
// }

// void AddVectors256_i64(const long * vecA, const long * vecB, long * output)
// {
//     __m256i result = _mm256_add_epi64(_mm256_loadu_epi64(vecA), _mm256_loadu_epi64(vecB));
// 	_mm256_storeu_epi64(output, result);
// }

void SubtractVectors256_f32(const float * vecA, const float * vecB, float * output)
{
    __m256 result = _mm256_sub_ps(_mm256_loadu_ps(vecA), _mm256_loadu_ps(vecB));
    _mm256_storeu_ps(output, result);
}

void SubtractVectors256_f64(const double * vecA, const double * vecB, double * output)
{
    __m256d result = _mm256_sub_pd(_mm256_loadu_pd(vecA), _mm256_loadu_pd(vecB));
	_mm256_storeu_pd(output, result);
}

// void SubtractVectors256_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m256i result = _mm256_sub_epi32(_mm256_loadu_epi32(vecA), _mm256_loadu_epi32(vecB));
// 	_mm256_storeu_epi32(output, result);
// }

// void SubtractVectors256_i64(const long * vecA, const long * vecB, long * output)
// {
//     __m256i result = _mm256_sub_epi64(_mm256_loadu_epi64(vecA), _mm256_loadu_epi64(vecB));
// 	_mm256_storeu_epi64(output, result);
// }

void MultiplyVectors256_f32(const float * vecA, const float * vecB, float * output)
{
    __m256 result = _mm256_mul_ps(_mm256_loadu_ps(vecA), _mm256_loadu_ps(vecB));
    _mm256_storeu_ps(output, result);
}

void MultiplyVectors256_f64(const double * vecA, const double * vecB, double * output)
{
    __m256d result = _mm256_mul_pd(_mm256_loadu_pd(vecA), _mm256_loadu_pd(vecB));
	_mm256_storeu_pd(output, result);
}

// void MultiplyVectors256_i32(const int * vecA, const int * vecB, int * output)
// {
//     __m256i result = _mm256_mul_epi32(_mm256_loadu_epi32(vecA), _mm256_loadu_epi32(vecB));
// 	_mm256_storeu_epi32(output, result);
// }

// void MultiplyVectors256_i64(const long * vecA, const long * vecB, long * output)
// {
// }
#endif

#ifdef _USE_AVX512
void AddVectors512_f32(const float * vecA, const float * vecB, float * output)
{
    __m512 result = _mm512_add_ps(_mm512_loadu_ps(vecA), _mm512_loadu_ps(vecB));
 	_mm512_storeu_ps(output, result);
}

void AddVectors512_f64(const double * vecA, const double * vecB, double * output)
{
    __m512d result = _mm512_add_pd(_mm512_loadu_pd(vecA), _mm512_loadu_pd(vecB));
 	_mm512_storeu_pd(output, result);
}

void AddVectors512_i32(const int * vecA, const int * vecB, int * output)
{
    __m512i result = _mm512_add_epi32(_mm512_loadu_epi32(vecA), _mm512_loadu_epi32(vecB));
 	_mm512_storeu_epi32(output, result);
}

void AddVectors512_i64(const long * vecA, const long * vecB, long * output)
{
    __m512i result = _mm512_add_epi64(_mm512_loadu_epi64(vecA), _mm512_loadu_epi64(vecB));
 	_mm512_storeu_epi64(output, result);
}

void SubtractVectors512_f32(const float * vecA, const float * vecB, float * output)
{
    __m512 result = _mm512_sub_ps(_mm512_loadu_ps(vecA), _mm512_loadu_ps(vecB));
 	_mm512_storeu_ps(output, result);
}

void SubtractVectors512_f64(const double * vecA, const double * vecB, double * output)
{
    __m512d result = _mm512_sub_pd(_mm512_loadu_pd(vecA), _mm512_loadu_pd(vecB));
 	_mm512_storeu_pd(output, result);
}

void SubtractVectors512_i32(const int * vecA, const int * vecB, int * output)
{
    __m512i result = _mm512_sub_epi32(_mm512_loadu_epi32(vecA), _mm512_loadu_epi32(vecB));
 	_mm512_storeu_epi32(output, result);
}

void SubtractVectors512_i64(const long * vecA, const long * vecB, long * output)
{
    __m512i result = _mm512_sub_epi64(_mm512_loadu_epi64(vecA), _mm512_loadu_epi64(vecB));
 	_mm512_storeu_epi64(output, result);
}

void MultiplyVectors512_f32(const float * vecA, const float * vecB, float * output)
{
    __m512 result = _mm512_mul_ps(_mm512_loadu_ps(vecA), _mm512_loadu_ps(vecB));
 	_mm512_storeu_ps(output, result);
}

void MultiplyVectors512_f64(const double * vecA, const double * vecB, double * output)
{
    __m512d result = _mm512_mul_pd(_mm512_loadu_pd(vecA), _mm512_loadu_pd(vecB));
 	_mm512_storeu_pd(output, result);
}

void MultiplyVectors512_i32(const int * vecA, const int * vecB, int * output)
{
    __m512i result = _mm512_mul_epi32(_mm512_loadu_epi32(vecA), _mm512_loadu_epi32(vecB));
 	_mm512_storeu_epi32(output, result);
}

// void MultiplyVectors512_i64(const long * vecA, const long * vecB, long * output)
// {
// }
#endif

#ifdef _USE_FMA256
void AddMultiplyVectors_f32(const float * vecA, const float * vecB, const float * vecC, float * output)
{
    __m256 result = _mm256_fmadd_ps(_mm256_loadu_ps(vecA), _mm256_loadu_ps(vecB), _mm256_loadu_ps(vecC));
	_mm256_storeu_ps(output, result);
}

void AddMultiplyVectors_f64(const double * vecA, const double * vecB, const double * vecC, double * output)
{
    __m256d result = _mm256_fmadd_pd(_mm256_loadu_pd(vecA), _mm256_loadu_pd(vecB), _mm256_loadu_pd(vecC));
	_mm256_storeu_pd(output, result);
}

// void AddMultiplyVectors_i32(const int * vecA, const int * vecB, const int * vecC, int * output) //TODO test if worth it.
// {
//     __m256i multResult = _mm256_mul_epi32(_mm256_loadu_epi32(vecA), _mm256_loadu_epi32(vecB));
//     __m256i result = _mm256_add_epi32(multResult, _mm256_loadu_epi32(vecC));
// 	_mm256_storeu_epi32(output, result);
// }

// void AddMultiplyVectors_i64(const long * vecA, const long * vecB, const long * vecC, long * output)
// {
// }

void SubtractMultiplyVectors_f32(const float * vecA, const float * vecB, const float * vecC, float * output)
{
    __m256 result = _mm256_fmsub_ps(_mm256_loadu_ps(vecA), _mm256_loadu_ps(vecB), _mm256_loadu_ps(vecC));
	_mm256_storeu_ps(output, result);
}

void SubtractMultiplyVectors_f64(const double * vecA, const double * vecB, const double * vecC, double * output)
{
    __m256d result = _mm256_fmsub_pd(_mm256_loadu_pd(vecA), _mm256_loadu_pd(vecB), _mm256_loadu_pd(vecC));
	_mm256_storeu_pd(output, result);
}

// void SubtractMultiplyVectors_i32(const int * vecA, const int * vecB, const int * vecC, int * output) //TODO test if worth it
// {
//     __m256i multResult = _mm256_mul_epi32(_mm256_loadu_epi32(vecA), _mm256_loadu_epi32(vecB));
//     __m256i result = _mm256_sub_epi32(multResult, _mm256_loadu_epi32(vecC));
// 	_mm256_storeu_epi32(output, result);
// }

// void SubtractMultiplyVectors_i64(const long * vecA, const long * vecB, const long * vecC, long * output)
// {
// }
#endif