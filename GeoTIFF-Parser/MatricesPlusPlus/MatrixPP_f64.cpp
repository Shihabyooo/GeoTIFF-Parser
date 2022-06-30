#include "MatrixPP_f64.hpp"

Matrix_f64::Matrix_f64()
{
	rows = 0;
	columns = 0;
	content = NULL;
}

Matrix_f64::Matrix_f64(_INDEX _rows, _INDEX _columns)
{
	Alloc(_rows, _columns);

	columns = _columns;
	rows = _rows;
}

Matrix_f64::Matrix_f64(_INDEX _rows, _INDEX _columns, double defaultValue)
{
	Alloc(_rows, _columns);

	columns = _columns;
	rows = _rows;

	SetEntireArrayToFixedValue(defaultValue);
}

Matrix_f64::Matrix_f64(const Matrix_f32 & sourceMat)
{
	*this = sourceMat;
}

Matrix_f64::Matrix_f64(const Matrix_f64 & sourceMat)
{
	*this = sourceMat;
}

Matrix_f64::Matrix_f64(Matrix_f64 && sourceMat)
{
	*this = std::move(sourceMat);
}

Matrix_f64::~Matrix_f64()
{
	DeleteContent();
}

Matrix_f64::Matrix_f64(Array2D<float> const & sourceArr)
{
	CopyFromArray2D(sourceArr);
}

Matrix_f64::Matrix_f64(Array2D<double> const & sourceArr)
{
	CopyFromArray2D(sourceArr);
}

Matrix_f64::Matrix_f64(Array2D<int> const & sourceArr)
{
	CopyFromArray2D(sourceArr);
}

Matrix_f64::Matrix_f64(Array2D<long> const & sourceArr)
{
	CopyFromArray2D(sourceArr);
}

Matrix_f64 & Matrix_f64::operator=(Matrix_f64 const & mat2)
{
	DeleteContent();

	rows = mat2.Rows();
	columns = mat2.Columns();

	if (mat2.content == NULL)
	{
		content = NULL;
		return *this;
	}

	Alloc(rows, columns);

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < columns; j++)
			content[i][j] = mat2.GetValue(i, j);

	return *this;
}

Matrix_f64 & Matrix_f64::operator=(Matrix_f64 && mat2)
{
	DeleteContent();
	rows = mat2.rows;
	columns = mat2.columns;
	content = mat2.content;

	mat2.content = NULL;
	mat2.rows = mat2.columns = 0;
	return *this;
}

Matrix_f64 Matrix_f64::operator*(const Matrix_f64 & mat2) const
{
#ifdef _VECTORIZED_CODE
	return MultiplyMatricesVectorized_N(*this, mat2);
#else
	return MultiplyMatrices(*this, mat2);
#endif
}

Matrix_f64 Matrix_f64::operator*(const double & scalar) const
{
#ifdef _VECTORIZED_CODE
	return MultiplayMatrixWithScalarVectorized(*this, scalar);
#else
	return MultiplayMatrixWithScalar(*this, scalar);
#endif
}

Matrix_f64 Matrix_f64::operator+(const Matrix_f64 & mat2) const
{
#ifdef _VECTORIZED_CODE
	return AddMatricesVectorized(*this, mat2);
#else
	return AddMatrices(*this, mat2);
#endif
}

Matrix_f64 Matrix_f64::operator-(const Matrix_f64 & mat2) const
{
#ifdef _VECTORIZED_CODE
	return SubtractMatricesVectorized(*this, mat2);
#else
	return SubtractMatrices(*this, mat2);
#endif
}

Matrix_f64 & Matrix_f64::operator+=(Matrix_f64 const & mat2)
{
#ifdef _VECTORIZED_CODE
	AddInPlaceVectorized(mat2);
#else
	AddInPlace(mat2);
#endif
	return *this;
}

Matrix_f64 & Matrix_f64::operator-=(Matrix_f64 const & mat2)
{
#ifdef _VECTORIZED_CODE
	SubtractInPlaceVectorized(mat2);
#else
	SubtractInPlace(mat2);
#endif
	return *this;
}

Matrix_f64 & Matrix_f64::operator*=(Matrix_f64 const & mat2)
{
#ifdef _VECTORIZED_CODE
	*this = MultiplyMatricesVectorized_N(*this, mat2);
#else
	*this = MultiplyMatrices(*this, mat2);
#endif
	return *this;
}

Matrix_f64 & Matrix_f64::operator*=(const double scalar)
{
#ifdef _VECTORIZED_CODE
	MultiplyWithScalarInPlaceVectorized(scalar);
#else
	MultiplyWithScalarInPlace(scalar);
#endif
	return *this;
}

Matrix_f64 Matrix_f64::Invert() const
{
	return InvertMatrix(*this);
}

double Matrix_f64::Determinant() const
{
	if (!IsSquared(*this))
	{
		std::cout << "ERROR! Attempting to calculate the determinant of a non-squared matrix." << std::endl;
		return 0;  //really need to figure out how to make this thing more gracefull.
	}

	return CalculateDeterminant(*this);
}

void Matrix_f64::Overlay(const Matrix_f64 mat2, _INDEX rowOffset, _INDEX columnOffset)
{
	if (content == NULL) //if this Array2D is uninitialized, this method acts as a simple assignment.
	{
		*this = mat2;
	}

	_INDEX _rows = rows > mat2.Rows() + rowOffset ? mat2.Rows() + rowOffset : rows;
	_INDEX _columns = columns > mat2.Columns() + columnOffset ? mat2.Columns() + columnOffset : columns;

	for (_INDEX i = rowOffset; i < _rows; i++)
	{
		for (_INDEX j = columnOffset; j < _columns; j++)
			content[i][j] += mat2.GetValue(i - rowOffset, j - columnOffset);
	}
}

Matrix_f64 ** Matrix_f64::DecomposeLU() const
{
	return DecomposeLU(*this);
}

Matrix_f64 ** Matrix_f64::DecomposeLUP() const
{
	return DecomposeLUP(*this);
}

bool Matrix_f64::NearlyEquals(const Matrix_f64 &mat2, double tolerance)
{
	return AreNearlyEquall(*this, mat2, tolerance);
}

bool Matrix_f64::IsSymmetric(double tolerance) const
{
	return IsSymmetric(*this, tolerance);
}

void Matrix_f64::AddInPlace(Matrix_f64 const & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, mat2))
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
#endif

	double * a = &content[0][0];
	double * b = &mat2.content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
	{
		*a += *b;
		a++;
		b++;
	}
}

void Matrix_f64::SubtractInPlace(Matrix_f64 const & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(*this, mat2))
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
#endif // _USE_BOUNDS_CHECK

	double * a = &content[0][0];
	double * b = &mat2.content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
	{
		*a -= *b;
		a++;
		b++;
	}
}

void Matrix_f64::MultiplyWithScalarInPlace(const double scalar)
{
	double * a = &content[0][0];
	size_t size = rows * columns;

	for (size_t i = 0; i < size; i++)
	{
		*a = *a * scalar;
		a++;
	}
}

#ifdef _VECTORIZED_CODE
void Matrix_f64::AddInPlaceVectorized(Matrix_f64 const & mat2)
{
	if (!AreOfSameSize(*this, mat2))
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;

	size_t size = rows * columns;

	double * a = &content[0][0];
	double * b = &mat2.content[0][0];

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
void Matrix_f64::SubtractInPlaceVectorized(Matrix_f64 const & mat2)
{
	if (!AreOfSameSize(*this, mat2))
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;

	size_t size = rows * columns;

	double * a = &content[0][0];
	double * b = &mat2.content[0][0];

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

void Matrix_f64::MultiplyWithScalarInPlaceVectorized(const double scalar)
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

Matrix_f64 Matrix_f64::Identity(_INDEX dimension)
{
	Matrix_f64 uMatrix(dimension, dimension);

	for (size_t i = 0; i < dimension; i++)
		uMatrix.SetValue(i, i, 1.0f);

	return uMatrix;
}

bool Matrix_f64::AreMultipliable(const Matrix_f64 &mat1, const Matrix_f64 &mat2)
{
	if (mat1.Columns() != mat2.Rows())
		return false;
	else
		return true;
}

bool Matrix_f64::IsInvertible(Matrix_f64 const & mat, bool checkSingular)
{
	if (mat.Rows() != mat.Columns())
		return false;
	else if (checkSingular && fabs(mat.Determinant()) <= MINDET) //Making use of the optimization where determinant computation wil be skipped if checkSingular is false
																//TODO check whether this is a standard and always the case, or is compiler/language version specific.
		return false;
	else
		return true;
}

bool Matrix_f64::IsSymmetric(const Matrix_f64 & mat, double tolerance)
{
	if (!IsSquared(mat))
		return false;

	tolerance = fabs(tolerance); //in case it was sent as a negative value, in-which case all matrices would turn out to be symmetric.

	for (_INDEX i = 0; i < mat.Rows(); i++)
	{
		for (_INDEX j = 0; j < mat.Rows(); j++)
		{
			if (fabs(mat.GetValue(i, j) - mat.GetValue(j, i)) >= tolerance)
				return false;
		}
	}

	return true;
}

bool Matrix_f64::AreNearlyEquall(const Matrix_f64 &mat1, const Matrix_f64 &mat2, double tolerance)
{
	tolerance = fabs(tolerance); //in case it was sent as a negative value, in-which case all matrices would turn out to be equall.
	if (!AreOfSameSize(mat1, mat2))
		return false;

	for (_INDEX i = 0; i < mat1.Rows(); i++)
	{
		for (_INDEX j = 0; j < mat1.Rows(); j++)
		{
			if (fabs(mat1.GetValue(i, j) - mat2.GetValue(i, j)) >= tolerance)
				return false;
		}
	}

	return true;
}

Matrix_f64 ** Matrix_f64::DecomposeLU(const Matrix_f64 & mat)
{
	//Based on the algorithm detailed on Introduction to Algorithms (3rd ed), Cormen, T., Lieserson, C., Rivest, R., and Stein, C.

	if (!IsSquared(mat))
	{
		std::cout << "ERROR! Cannot decompose a non-squared matrix." << std::endl;
		return NULL;
	}

	Matrix_f64 ** decomposition = new Matrix_f64 *[2]; //first is lower, second is upper.

	Matrix_f64 * lower = decomposition[0] = new Matrix_f64(mat.Rows(), mat.Rows());
	Matrix_f64 * upper = decomposition[1] = new Matrix_f64(mat.Rows(), mat.Rows());

	//we need a temporary matrix initially holding the same content of original matrix for the computations bellow
	Matrix_f64 * tempMatrix = new Matrix_f64(mat);

	for (int i = 0; i < mat.Rows(); i++)
	{
		//upper diagonal is same for original matrix
		upper->SetValue(i, i, tempMatrix->GetValue(i, i));

		//lower diagonal = 1
		lower->SetValue(i, i, 1.0f);

		for (int j = i + 1; j < mat.Rows(); j++)
		{
			//lower is divided by upper's pivot
			lower->SetValue(j, i, tempMatrix->GetValue(j, i) / upper->GetValue(i, i));

			//upper triangle (above diagonal) is the Schur compliment
			upper->SetValue(i, j, tempMatrix->GetValue(i, j));
		}

		//compute the Schur complement in-place in the temporary matrix
		for (int k = i + 1; k < mat.Rows(); k++)
		{
			for (int l = i + 1; l < mat.Rows(); l++)
			{
				tempMatrix->SetValue(k, l, tempMatrix->GetValue(k, l) - lower->GetValue(k, i) * upper->GetValue(i, l));
			}
		}
	}

	//cleanup and return
	delete tempMatrix;

	return decomposition;
}

Matrix_f64 ** Matrix_f64::DecomposeLUP(const Matrix_f64 &mat) //TODO finish implementing this
{
	//Based on the algorithm detailed on Introduction to Algorithms (3rd ed), Cormen, T., Lieserson, C., Rivest, R., and Stein, C.

	if (!IsSquared(mat))
	{
		std::cout << "ERROR! Cannot decompose a non-squared matrix." << std::endl;
		return NULL;
	}

	Matrix_f64 ** decomposition = new Matrix_f64 *[3]; //first is lower, second is upper, third is permutation

	return decomposition;
}

Matrix_f64 Matrix_f64::AddMatrices(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Matrix_f64 result(mat1.Rows(), mat1.Columns());
	size_t size = result.Rows() * result.Columns();

	double * a = &mat1.content[0][0];
	double * b = &mat2.content[0][0];
	double * c = &result.content[0][0];

	for (size_t i = 0; i < size; i++)
	{
		*c = *a + *b;
		a++;
		b++;
		c++;
	}

	return result;
}

Matrix_f64 Matrix_f64::SubtractMatrices(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Matrix_f64 result(mat1.Rows(), mat1.Columns());
	size_t size = result.Rows() * result.Columns();

	double * a = &mat1.content[0][0];
	double * b = &mat2.content[0][0];
	double * c = &result.content[0][0];

	for (size_t i = 0; i < size; i++)
	{
		*c = *a - *b;
		a++;
		b++;
		c++;
	}

	return result;
}

Matrix_f64 Matrix_f64::MultiplyMatrices(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreMultipliable(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to multiply array of " << mat1.Columns() << "columns with an array of " << mat2.Rows() << " rows." << std::endl;
		return Matrix_f64();  //really need to figure out how to make this thing more gracefull.
	}
#endif

	Matrix_f64 result(mat1.Rows(), mat2.Columns());

	for (_INDEX i = 0; i < mat1.Rows(); i++)
	{
		for (_INDEX j = 0; j < mat2.Columns(); j++)
		{
			double cellValue = 0.0f;

			for (_INDEX k = 0; k < mat1.Columns(); k++)
				cellValue += mat1.GetValue(i, k) * mat2.GetValue(k, j);

			result.SetValue(i, j, cellValue);
		}
	}

	return result;
}

Matrix_f64 Matrix_f64::MultiplayMatrixWithScalar(const Matrix_f64 & mat1, const double scalar)
{
	Matrix_f64 result(mat1.Rows(), mat1.Columns());
	size_t size = result.Rows() * result.Columns();

	double * a = &mat1.content[0][0];
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
Matrix_f64 Matrix_f64::AddMatricesVectorized(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Matrix_f64 result(mat1.Rows(), mat1.Columns());

	size_t size = result.Rows() * result.Columns();

	double * a = &mat1.content[0][0];
	double * b = &mat2.content[0][0];
	double * c = &result.content[0][0];

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
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


Matrix_f64 Matrix_f64::SubtractMatricesVectorized(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreOfSameSize(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif

	Matrix_f64 result(mat1.Rows(), mat1.Columns());
	size_t size = result.Rows() * result.Columns();

	double * a = &mat1.content[0][0];
	double * b = &mat2.content[0][0];
	double * c = &result.content[0][0];

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
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

Matrix_f64 Matrix_f64::MultiplyMatricesVectorized_N(const Matrix_f64 & mat1, const Matrix_f64 & mat2)
{
#ifdef _USE_BOUNDS_CHECK
	if (!AreMultipliable(mat1, mat2))
	{
		std::cout << "ERROR! Attempting to add arrays of different sizes." << std::endl;
		return Matrix_f64();
	}
#endif
	if (mat1.Rows() < _VECTOR_SIZE_F64) //no need to vectorize
		return MultiplyMatrices(mat1, mat2); //TODO move this check to * overload

	Matrix_f64 result(mat1.Rows(), mat2.Columns());

	size_t size = result.Rows() * result.Columns();

	int remainder = mat1.Columns() % _VECTOR_SIZE_F64;
	int vectorizedSpan = mat1.Columns() - remainder;

	//std::cout << "Size: " << size << " - Remainder: " << remainder << " - Vectorized Span: " << vectorizedSpan << std::endl; //test

	double * a = new double[_VECTOR_SIZE_F64];
	double * b = new double[_VECTOR_SIZE_F64];
	double * c = new double[_VECTOR_SIZE_F64];

	for (_INDEX i = 0; i < size; i++)
	{
		_INDEX row = floor(i / result.Rows());
		_INDEX column = i % result.Columns();
		for (_INDEX j = 0; j < vectorizedSpan; j += _VECTOR_SIZE_F64)
		{
#define EXPAND(x) a[x]=mat1.content[row][j+x];\
			b[x]=mat2.content[j+x][column];\

			EXPAND(0)
				EXPAND(1)
				EXPAND(2)
				EXPAND(3)
#ifdef _USE_AVX256
				EXPAND(4)
				EXPAND(5)
				EXPAND(6)
				EXPAND(7)
#endif
#ifdef _USE_AVX512
				EXPAND(8)
				EXPAND(9)
				EXPAND(10)
				EXPAND(11)
				EXPAND(12)
				EXPAND(13)
				EXPAND(14)
				EXPAND(15)
#endif

#ifdef _USE_AVX512
				MultiplyVectors512_f64(a, b, c);
#elif defined(_USE_AVX256)
				MultiplyVectors256_f64(a, b, c);
#elif defined(_USE_SSE)
				MultiplyVectors128_f64(a, b, c);
#endif

			result[row][column] += c[0] + c[1] + c[2] + c[3];
#ifdef _USE_AVX256
			result[row][column] += c[4] + c[5] + c[6] + c[7];
#endif
#ifdef _USE_AVX512
			result[row][column] += c[8] + c[9] + c[10] + c[11] + c[12] + c[13] + c[14] + c[15]
#endif
		}
		for (_INDEX j = vectorizedSpan; j < vectorizedSpan + remainder; j++)
		{
			result[row][column] += mat1.GetValue(row, j) * mat2.GetValue(j, column);
		}
	}

	delete[] a;
	delete[] b;
	delete[] c;

	return result;
}

Matrix_f64 Matrix_f64::MultiplayMatrixWithScalarVectorized(const Matrix_f64 & mat1, const double scalar)
{
	Matrix_f64 result(mat1.Rows(), mat1.Columns());

	_INDEX size = mat1.Rows() * mat1.Columns();
	double * a = &mat1.content[0][0];
	double * b;
	double * c = &result.content[0][0];

	b = new double[_VECTOR_SIZE_F64];
	for (int i = 0; i < _VECTOR_SIZE_F64; i++)
		b[i] = scalar;

	for (_INDEX i = 0; i < size; i += _VECTOR_SIZE_F64)
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

Matrix_f64 Matrix_f64::InvertMatrix(const Matrix_f64 & sourceMat, MatrixInversionMethod method)
{
#ifdef _USE_BOUNDS_CHECK
	if (!IsInvertible(sourceMat))
	{
		std::cout << "ERROR! Attempting to invert a non-invertible array." << std::endl;
		return Matrix_f64();
	}
#endif

	switch (method)
	{
	case MatrixInversionMethod::Gauss_Jordan:
		return GausJordanElimination(sourceMat);
	case MatrixInversionMethod::Blockwise:
		return BlockwiseInversion(sourceMat);
	default:
		std::cout << "ERROR! Received unsupported MatrixInversionMethod in InverArray()." << std::endl; //shouldn't happen (but still...)
		return Matrix_f64();
	}
}

double Matrix_f64::CalculateDeterminant(const Matrix_f64 & mat)
{
	//For 1x1, 2x2 and 3x3 matrices, the determinant is returned using hardcoded formula.
	//For larger matrices, LU decomposition is made use of, where for A = LU -> det(A) = det(L) * det(U),
	//and for triangular matrices, the determinant is simply the product of the diagonal.

	//TODO add special case for 4x4 matrix
	switch (mat.Rows())
	{
	case 1:
		return mat.GetValue(0, 0);
	case 2:
		return (mat.GetValue(0, 0) * mat.GetValue(1, 1)) - (mat.GetValue(1, 0) * mat.GetValue(0, 1));
	case 3:
		return (mat.GetValue(0, 0) * (mat.GetValue(1, 1) * mat.GetValue(2, 2) - mat.GetValue(1, 2) * mat.GetValue(2, 1))
			- mat.GetValue(0, 1) * (mat.GetValue(1, 0) * mat.GetValue(2, 2) - mat.GetValue(1, 2) * mat.GetValue(2, 0))
			+ mat.GetValue(0, 2) * (mat.GetValue(1, 0) * mat.GetValue(2, 1) - mat.GetValue(1, 1) * mat.GetValue(2, 0)));
		break;
	default:
		break;
	}

	Matrix_f64 ** LU = mat.DecomposeLU();

	if (LU == NULL)
	{
		std::cout << "ERROR! Could not generate LU decomposition to computedeterminant." << std::endl;
		return 0;
	}

	double result = LU[0]->GetValue(0, 0) * LU[1]->GetValue(0, 0);
	for (_INDEX i = 0; i < mat.Rows(); i++)
		result = result * LU[0]->GetValue(i, i) * LU[1]->GetValue(i, i);

	delete LU[0];
	delete LU[1];
	delete[] LU;

	return result;
}

template <typename T>
void Matrix_f64::CopyFromArray2D(Array2D<T> const & sourceArr)
{
	DeleteContent();

	rows = sourceArr.Rows();
	columns = sourceArr.Columns();

	if (sourceArr.IsEmpty())
	{
		content = NULL;
		return;
	}

	Alloc(rows, columns);

	for (_INDEX i = 0; i < rows; i++)
		for (_INDEX j = 0; j < columns; j++)
			content[i][j] = static_cast<double>(sourceArr.GetValue(i, j));
}

void Matrix_f64::AllocateMemory(_INDEX _rows, _INDEX _columns)
{
#ifndef _VECTORIZED_CODE
#define _VECTOR_SIZE_F64 1
#endif // !_VECTORIZED_CODE

	try
	{
		size_t rawSize = _rows * _columns;

		size_t paddedSize = rawSize;
		if (rawSize%_VECTOR_SIZE_F64 > 0)
			paddedSize = rawSize + _VECTOR_SIZE_F64 - (rawSize%_VECTOR_SIZE_F64);

#ifndef _VECTORIZED_CODE
#undef _VECTOR_SIZE_F64
#endif // !_VECTORIZED_CODE

		content = new double*[_rows];
		double * helperPtr = new double[paddedSize]();

		for (_INDEX i = 0; i < _rows; i++)
		{
			content[i] = helperPtr;
			helperPtr += _columns;
		}
	}
	catch (const std::bad_alloc& exception)
	{
		std::cout << "ERROR! Could not allocate array. " << exception.what() << std::endl;
		throw exception;
	}
}

Matrix_f64 Matrix_f64::GausJordanElimination(const Matrix_f64 & sourceMat)
{
	Matrix_f64 result(sourceMat.Rows(), sourceMat.Columns());
	//TODO the line bellow allocates the memory twice unneessarily (ones when casting to Matrix_f64, and once when assigning to augmentedArr.
	//This can be avoided by redoing the structure and nix the ArrayPP and inherentence, or moving MergeArrays locally, which would make the
	//code more redundant, and thus going more towards the first solution (while having double the LoC -_-).
	Matrix_f64 augmentedArr = static_cast<Matrix_f64>(MergeArrays(sourceMat, Identity(sourceMat.Rows()))); //augmentedArr is the augment matrix, which is the original matrix with a identity matrix attached to its right.

	//If first pivot value is zero, must swap the row with another that has a non-zero value. If none exist, can't use this method.
	if (augmentedArr.GetValue(0, 0) == 0.0f)
	{
		//look for a row where first element is not zero
		for (int i = 0; i < augmentedArr.Rows(); i++)
		{
			if (augmentedArr.GetValue(i, 0) != 0.0f)
			{
				augmentedArr.SwapRows(0, i);
				break;
			}
			if (i == augmentedArr.Rows())
			{
				std::cout << "ERROR! Could not find a suitable pivot value for first row. Can't invert useing Gauss-Jordan Elimmination" << std::endl;
				return Matrix_f64();
			}
		}
	}

	//For an array of n*n size, n steps are needed to get the inverse.
	for (size_t step = 0; step < result.Rows(); step++) //the variable "step" here will be our pivot row, and by virtue of being a squared array, our pivot column as well.
	{
		double pivotValue = augmentedArr.GetValue(step, step);  //the value of th pivot cell is always on the diagonal.

		//mFactor is the value that, when multiplied with our pivot row and substracted from current row, should help reduce it towards zero (except diagonal value)
		//we extract mFactors before the loops because, their cell values will change inside the loops at first iterations, but we'll still be needing them for remaining iterations.
		double * mFactors = new double[result.Rows()];
		for (size_t i = 0; i < augmentedArr.Rows(); i++)
			mFactors[i] = augmentedArr.GetValue(i, step);

		for (size_t j = 0; j < augmentedArr.Columns(); j++)
		{
			double newColumnValueAtPivotRow = augmentedArr.GetValue(step, j) / pivotValue;
			augmentedArr.SetValue(step, j, newColumnValueAtPivotRow); //the pivot row value is adjusted before iterating over other rows.

			for (size_t i = 0; i < augmentedArr.Rows(); i++) //we then iterate over other rows, using the if-statement bellow to avoid modifying our pivot row.
			{
				if (i != step)
				{
					double newValueAtOtherRow = augmentedArr.GetValue(i, j) - (mFactors[i] * newColumnValueAtPivotRow);
					augmentedArr.SetValue(i, j, newValueAtOtherRow);
				}
				//augmentedArr.DisplayArrayInCLI();
			}
		}
		delete[] mFactors;
	}

	//extract result from augmentedArr
	result = static_cast<Matrix_f64>(augmentedArr.GetSubMatrix(0, result.Rows(), sourceMat.Columns(), result.Columns()));

	return result;
}

Matrix_f64 Matrix_f64::BlockwiseInversion(const Matrix_f64 & sourceMat) //recurisve function
{

	//https://en.wikipedia.org/wiki/Invertible_matrix#Blockwise_inversion
	//https://math.stackexchange.com/questions/2735/solving-very-large-matrices-in-pieces

	//Subdivide matrix A into 	[E	F]
	//							[G	H]

	//Inv[A] then is			[E' + (E' F)(H - G E' F)'(G E')		-(E' F)(H - G E' F)']
	//							[-(H - G E' F)'						(H - G E' F)'		]

	//Note: This implementation is atrociously memory-inefficient. It seems that it's possible to some (msot?) of the processing in place. To be further investigated.
	//https://math.stackexchange.com/questions/16940/in-place-inversion-of-large-matrices
	//copy the sourceMat into result, then operate on result.

	switch (sourceMat.Rows())
	{
	case 1:
		return Invert1x1Matrix(sourceMat);
	case 2:
		return Invert2x2Matrix(sourceMat);
	case 3:
		return Invert3x3Matrix(sourceMat);
	default:
		break;
	}

	_INDEX primarySubDimension = round((double)sourceMat.Rows() / 2.0F);
	////You don't actually need to store e itself, could directly pass it to next recursion step.
	Matrix_f64 e_ = BlockwiseInversion(static_cast<Matrix_f64>(sourceMat.GetSubMatrix(0, primarySubDimension, 0, primarySubDimension)));

	Matrix_f64 f = static_cast<Matrix_f64>(sourceMat.GetSubMatrix(0, primarySubDimension, primarySubDimension, sourceMat.Columns() - primarySubDimension));
	Matrix_f64 g = static_cast<Matrix_f64>(sourceMat.GetSubMatrix(primarySubDimension, sourceMat.Rows() - primarySubDimension, 0, primarySubDimension));
	Matrix_f64 h = static_cast<Matrix_f64>(sourceMat.GetSubMatrix(primarySubDimension, sourceMat.Rows() - primarySubDimension, primarySubDimension, sourceMat.Columns() - primarySubDimension));

	//Matrix_f64 e_ = BlockwiseInversion(e);
	Matrix_f64 e_f = e_ * f;
	Matrix_f64 ge_ = g * e_;
	Matrix_f64 hge_f_ = BlockwiseInversion(h - (g * e_) * f);

	//std::cout << "inside recurisve func for array of size: " << sourceMat.Rows() << std::endl;
	// sourceMat.GetSubMatrix(0, primarySubDimension, 0, primarySubDimension).DisplayArrayInCLI();
	// f.DisplayArrayInCLI();
	// g.DisplayArrayInCLI();
	// h.DisplayArrayInCLI();
	// ((Matrix_f64)sourceMat.GetSubMatrix(0, primarySubDimension, 0, primarySubDimension) * e_).DisplayArrayInCLI();
	// ((h - (g * e_) * f) * hge_f_).DisplayArrayInCLI();


	Matrix_f64 result = static_cast<Matrix_f64>(StackArrays(
		Matrix_f64::MergeArrays(e_ + e_f * hge_f_ * ge_, (e_f * -1.0f) * hge_f_),
		Matrix_f64::MergeArrays((hge_f_ * -1.0f) * ge_, hge_f_)));

	// (e_ + e_f * hge_f_ * ge_).DisplayArrayInCLI();
	// ((e_f * -1.0f) * hge_f_).DisplayArrayInCLI();
	// ((hge_f_ * -1.0f) * ge_ ).DisplayArrayInCLI();
	// (hge_f_).DisplayArrayInCLI();
	// result.DisplayArrayInCLI();

	return result;
}

Matrix_f64 Matrix_f64::Invert1x1Matrix(const Matrix_f64 & sourceMat)
{
	return Matrix_f64(1, 1, 1.0f / sourceMat.GetValue(0, 0));
}

Matrix_f64 Matrix_f64::Invert2x2Matrix(const Matrix_f64 & sourceMat)
{
	Matrix_f64 inv = Matrix_f64(2, 2);
	double det = CalculateDeterminant(sourceMat);

	inv[0][0] = sourceMat.GetValue(1, 1) / det;
	inv[1][0] = -1.0f * sourceMat.GetValue(1, 0) / det;
	inv[0][1] = -1.0f * sourceMat.GetValue(0, 1) / det;
	inv[1][1] = sourceMat.GetValue(0, 0) / det;

	return inv;
}

Matrix_f64 Matrix_f64::Invert3x3Matrix(const Matrix_f64 & sourceMat)
{
	Matrix_f64 inv = Matrix_f64(3, 3);
	double det = CalculateDeterminant(sourceMat);

	inv[0][0] = (sourceMat.GetValue(1, 1) * sourceMat.GetValue(2, 2) - sourceMat.GetValue(1, 2) * sourceMat.GetValue(2, 1)) / det;
	inv[0][1] = (sourceMat.GetValue(0, 2) * sourceMat.GetValue(2, 1) - sourceMat.GetValue(0, 1) * sourceMat.GetValue(2, 2)) / det;
	inv[0][2] = (sourceMat.GetValue(0, 1) * sourceMat.GetValue(1, 2) - sourceMat.GetValue(0, 2) * sourceMat.GetValue(1, 1)) / det;

	inv[1][0] = (sourceMat.GetValue(1, 2) * sourceMat.GetValue(2, 0) - sourceMat.GetValue(1, 0) * sourceMat.GetValue(2, 2)) / det;
	inv[1][1] = (sourceMat.GetValue(0, 0) * sourceMat.GetValue(2, 2) - sourceMat.GetValue(0, 2) * sourceMat.GetValue(2, 0)) / det;
	inv[1][2] = (sourceMat.GetValue(0, 2) * sourceMat.GetValue(1, 0) - sourceMat.GetValue(0, 0) * sourceMat.GetValue(1, 2)) / det;

	inv[2][0] = (sourceMat.GetValue(1, 0) * sourceMat.GetValue(2, 1) - sourceMat.GetValue(1, 1) * sourceMat.GetValue(2, 0)) / det;
	inv[2][1] = (sourceMat.GetValue(0, 1) * sourceMat.GetValue(2, 0) - sourceMat.GetValue(0, 0) * sourceMat.GetValue(2, 1)) / det;
	inv[2][2] = (sourceMat.GetValue(0, 0) * sourceMat.GetValue(1, 1) - sourceMat.GetValue(0, 1) * sourceMat.GetValue(1, 0)) / det;

	return inv;
}