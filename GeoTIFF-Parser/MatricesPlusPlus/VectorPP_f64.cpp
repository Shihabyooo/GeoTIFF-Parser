#include "VectorPP_f64.hpp"

Vector_f64::Vector_f64()
{
	rows = 0;
	columns = 1;
	content = NULL;
}

Vector_f64::Vector_f64(_INDEX const size)
{
	Alloc(size, 1);

	columns = 1;
	rows = size;
}

Vector_f64::Vector_f64(_INDEX const size, double defaultValue)
{
	Alloc(size, 1);

	columns = 1;
	rows = size;
	SetEntireArrayToFixedValue(defaultValue);
}

Vector_f64::Vector_f64(Vector_f32 const & sourceVec)
{
	*this = sourceVec;
}

Vector_f64::Vector_f64(Vector_f64 const & sourceVec)
{
	*this = sourceVec;
}

Vector_f64::Vector_f64(Vector_f64 && sourceVec)
{
	*this = std::move(sourceVec);
}

Vector_f64::Vector_f64(Matrix_f32 const & sourceMat, _INDEX column)
{
	VectorFromMatrix(sourceMat, column);
}

Vector_f64::Vector_f64(Matrix_f64 const & sourceMat, _INDEX column)
{
	VectorFromMatrix(sourceMat, column);
}

Vector_f64::~Vector_f64()
{
	DeleteContent();
}

Vector_f64 & Vector_f64::operator=(Vector_f32 const & vec2)
{
	DeleteContent();

	rows = vec2.Rows();
	columns = vec2.Columns();

	if (vec2.Rows() < 1)
	{
		content = NULL;
		return *this;
	}

	Alloc(rows, columns);

	for (size_t i = 0; i < rows; i++)
		content[i][0] = vec2.GetValue(i);

	return *this;
}

Vector_f64 & Vector_f64::operator=(Vector_f64 const & vec2)
{
	DeleteContent();

	rows = vec2.Rows();
	columns = vec2.Columns();

	if (vec2.content == NULL)
	{
		content = NULL;
		return *this;
	}

	Alloc(rows, columns);

	for (size_t i = 0; i < rows; i++)
		content[i][0] = vec2.GetValue(i);

	return *this;
}

Vector_f64 & Vector_f64::operator=(Vector_f64 && vec2)
{
	DeleteContent();

	rows = vec2.rows;
	columns = vec2.columns;
	content = vec2.content;

	vec2.content = NULL;
	vec2.rows = vec2.columns = 0;

	return *this;
}

Vector_f64 Vector_f64::operator+(Vector_f64 const & vec2) const
{
#ifdef _VECTORIZED_CODE
	return AddVectorsVectorized(*this, vec2);
#else
	return AddVectors(*this, vec2);
#endif
}

Vector_f64 Vector_f64::operator-(Vector_f64 const & vec2) const
{
#ifdef _VECTORIZED_CODE
	return SubtractVectorsVectorized(*this, vec2);
#else
	return SubtractVectors(*this, vec2);
#endif
}

Vector_f64 Vector_f64::operator*(double const scalar)
{
	return MultiplayVectorWithScalar(*this, scalar);
}

Vector_f64 & Vector_f64::operator+=(Vector_f64 const & vec2)
{
#ifdef _VECTORIZED_CODE
	AddInPlaceVectorized(vec2);
#else
	AddInPlace(vec2);
#endif
	return *this;
}

Vector_f64 & Vector_f64::operator-=(Vector_f64 const & vec2)
{
#ifdef _VECTORIZED_CODE
	SubtractInPlaceVectorized(vec2);
#else
	SubtractInPlace(vec2);
#endif
	return *this;
}

Vector_f64 & Vector_f64::operator*=(const double scalar)
{
#ifdef _VECTORIZED_CODE
	MultiplyWithScalarInPlaceVectorized(scalar);
#else
	MultiplyWithScalarInPlace(scalar);
#endif
	return *this;
}

double & Vector_f64::operator[](const _INDEX row)
{
#ifdef _USE_BOUNDS_CHECK
	if (content == NULL			//Checking whether this object is empty. Making an assumption that initializing the first level of the content is automatically followed by init of sublevel.
		|| row >= rows)		//Checking out-of-bound writes.
		throw std::out_of_range("ERROR! Row value out of range or content set to NULL");
#endif

	return content[row][0];
}

double Vector_f64::operator[](const _INDEX row) const
{
#ifdef _USE_BOUNDS_CHECK
	if (content == NULL			//Checking whether this object is empty. Making an assumption that initializing the first level of the content is automatically followed by init of sublevel.
		|| row >= rows)		//Checking out-of-bound writes.
		throw std::out_of_range("ERROR! Row value out of range or content set to NULL");
#endif

	return content[row][0];
}


double Vector_f64::GetValue(_INDEX row) const
{
	return content[row][0];
}

double Vector_f64::Magnitude()
{
	double mag = 0;
	for (_INDEX i = 0; i < rows; i++)
		mag += pow(content[i][0], 2);
	return sqrt(mag);
}

double Vector_f64::DotProduct(Vector_f64 const & vec2) const
{
	return DotProduct(*this, vec2);
}

void Vector_f64::AddInPlace(Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, vec2))
		std::cout << "ERROR! Attempting to add vectors of different sizes." << std::endl;
#endif

	double * a = &content[0][0];
	double * b = &vec2.content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
	{
		*a += *b;
		a++;
		b++;
	}
}

void Vector_f64::SubtractInPlace(Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, vec2))
		std::cout << "ERROR! Attempting to add vectors of different sizes." << std::endl;
#endif

	double * a = &content[0][0];
	double * b = &vec2.content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
	{
		*a -= *b;
		a++;
		b++;
	}
}

void Vector_f64::MultiplyWithScalarInPlace(const double scalar)
{
	double * a = &content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
		*a = *a * scalar;
}

#ifdef _VECTORIZED_CODE
void Vector_f64::AddInPlaceVectorized(Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, vec2))
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
#endif

	size_t size = rows * columns;

	double * a = &content[0][0];
	double * b = &vec2.content[0][0];

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
	{
#ifdef _USE_AVX512
		AddVectors512_f64(a, b, a);
#elif defined(_USE_AVX256)
		AddVectors256_f64(a, b, a);
#elif defined(_USE_SSE)
		AddVectors128_f64(a, b, a);
#endif

		a += _VECTOR_SIZE_F64;
		b += _VECTOR_SIZE_F64;
	}
}

void Vector_f64::SubtractInPlaceVectorized(Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, vec2))
		std::cout << "ERROR! Attempting to add vectors of different sizes." << std::endl;
#endif

	size_t size = rows * columns;

	double * a = &content[0][0];
	double * b = &vec2.content[0][0];

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
	{
#ifdef _USE_AVX512
		SubtractVectors512_f64(a, b, a);
#elif defined(_USE_AVX256)
		SubtractVectors256_f64(a, b, a);
#elif defined(_USE_SSE)
		SubtractVectors128_f64(a, b, a);
#endif

		a += _VECTOR_SIZE_F64;
		b += _VECTOR_SIZE_F64;
	}
}

void Vector_f64::MultiplyWithScalarInPlaceVectorized(const double scalar)
{
	size_t size = rows * columns;

	double * a = &content[0][0];
	double * b = new double[_VECTOR_SIZE_F64];
	for (int i = 0; i < _VECTOR_SIZE_F64; i++)
		b[i] = scalar;

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
	{
#ifdef _USE_AVX512
		MultiplyVectors512_f64(a, b, a);
#elif defined(_USE_AVX256)
		MultiplyVectors256_f64(a, b, a);
#elif defined(_USE_SSE)
		MultiplyVectors128_f64(a, b, a);
#endif

		a += _VECTOR_SIZE_F64;
	}
	delete[] b;
}

#endif

Vector_f64 Vector_f64::AddVectors(Vector_f64 const & vec1, Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(vec1, vec1))
	{
		std::cout << "ERROR! Attempting to add vectors of different sizes." << std::endl;
		return Vector_f64();
	}
#endif

	Vector_f64 result(vec1.Rows());

	double * a = &vec1.content[0][0];
	double * b = &vec2.content[0][0];
	double * c = &result.content[0][0];

	for (size_t i = 0; i < result.rows; i++)
	{
		*c = *a + *b;
		a++;
		b++;
		c++;
	}

	return result;
}

Vector_f64 Vector_f64::SubtractVectors(Vector_f64 const & vec1, Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(vec1, vec1))
	{
		std::cout << "ERROR! Attempting to add vectors of different sizes." << std::endl;
		return Vector_f64();
	}
#endif

	Vector_f64 result(vec1.Rows());

	double * a = &vec1.content[0][0];
	double * b = &vec2.content[0][0];
	double * c = &result.content[0][0];

	for (size_t i = 0; i < result.rows; i++)
	{
		*c = *a - *b;
		a++;
		b++;
		c++;
	}

	return result;
}

Vector_f64 Vector_f64::MultiplayVectorWithScalar(Vector_f64 const & vec1, double const scalar)
{
	Vector_f64 result(vec1.Rows(), vec1.Columns());
	size_t size = result.Rows() * result.Columns();

	double * a = &vec1.content[0][0];
	double * b = &result.content[0][0];

	for (size_t i = 0; i < size; i++)
	{
		*b = *a * scalar;
		a++;
		b++;
	}

	return result;
}

#ifdef _VECTORIZED_CODE

Vector_f64 Vector_f64::AddVectorsVectorized(Vector_f64 const & vec1, Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Vector_f64 result(vec1.Rows());

	double * a = &vec1.content[0][0];
	double * b = &vec2.content[0][0];
	double * c = &result.content[0][0];

	for (_INDEX i = 0; i < result.Rows(); i += _VECTOR_SIZE_F64)
	{
#ifdef _USE_AVX512
		AddVectors512_f64(a, b, c);
#elif defined(_USE_AVX256)
		AddVectors256_f64(a, b, c);
#elif defined(_USE_SSE)
		AddVectors128_f64(a, b, c);
#endif

		a += _VECTOR_SIZE_F64;
		b += _VECTOR_SIZE_F64;
		c += _VECTOR_SIZE_F64;
	}

	return result;
}

Vector_f64 Vector_f64::SubtractVectorsVectorized(Vector_f64 const & vec1, Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Vector_f64 result(vec1.Rows());

	double * a = &vec1.content[0][0];
	double * b = &vec2.content[0][0];
	double * c = &result.content[0][0];

	for (_INDEX i = 0; i < result.Rows(); i += _VECTOR_SIZE_F64)
	{
#ifdef _USE_AVX512
		SubtractVectors512_f64(a, b, c);
#elif defined(_USE_AVX256)
		SubtractVectors256_f64(a, b, c);
#elif defined(_USE_SSE)
		SubtractVectors128_f64(a, b, c);
#endif

		a += _VECTOR_SIZE_F64;
		b += _VECTOR_SIZE_F64;
		c += _VECTOR_SIZE_F64;
	}

	return result;
}

Vector_f64 Vector_f64::MultiplayVectorScalarVectorized(Vector_f64 const & vec1, double const scalar)
{
	Vector_f64 result(vec1.Rows());

	double * a = &vec1.content[0][0];
	double * b;
	double * c = &result.content[0][0];

	b = new double[_VECTOR_SIZE_F64];
	for (int i = 0; i < _VECTOR_SIZE_F64; i++)
		b[i] = scalar;

	for (_INDEX i = 0; i < vec1.Rows(); i += _VECTOR_SIZE_F64)
	{
#ifdef  _USE_AVX512
		MultiplyVectors512_f64(a, b, c);
#elif defined(_USE_AVX256)
		MultiplyVectors256_f64(a, b, c);
#elif defined(_USE_SSE)
		MultiplyVectors128_f64(a, b, c);
#endif

		a += _VECTOR_SIZE_F64;
		c += _VECTOR_SIZE_F64;
	}

	delete[] b;
	return result;
}

#endif

double Vector_f64::DotProduct(Vector_f64 const & vec1, Vector_f64 const & vec2)
{
#ifdef _USE_BOUNDS_CHECK
	if (vec1.rows != vec2.rows)
	{
		std::cout << "ERROR! Attempting to multiply vectors of different sizes." << std::endl;
		return 0.0;
	}
#endif // _USE_BOUNDS_CHECK

	//TODO figure out a solution to the extra memory copy (from vec1.transpose() to Matrix_f64).
	return (static_cast<Matrix_f64>(vec1.Transpose()) * vec2)[0][0];
}

void Vector_f64::VectorFromMatrix(Matrix_f64 const & sourceMat, _INDEX column)
{
	DeleteContent();
	rows = sourceMat.Rows();
	columns = 1;
	Alloc(rows, 1);

	for (int i = 0; i < rows; i++)
		content[i][0] = sourceMat.GetValue(i, column);
}