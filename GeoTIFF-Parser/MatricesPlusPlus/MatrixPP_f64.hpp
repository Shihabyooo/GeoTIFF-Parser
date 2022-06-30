#pragma once
#include "ArrayPP.hpp"
#include "VectorMaths.hpp"
#include "MatrixPP_f32.hpp"

//instaniate necessary versions of the Array2D template.
//template class Array2D<float>;
template class Array2D<double>;
//template class Array2D<int>;
//template class Array2D<long>;

class Matrix_f64 : public Array2D<double> {
public:
	//using Array2D<double>::Array2D; //To inherit base class constructors.
	//constructors and destructors
	Matrix_f64();
	Matrix_f64(_INDEX _rows, _INDEX _columns);
	Matrix_f64(_INDEX _rows, _INDEX _columns, double defaultValue);
	Matrix_f64(const Matrix_f32 & sourceMat); //Copy constructor (Deep copy)
	Matrix_f64(const Matrix_f64 & sourceMat); //Copy constructor (Deep copy)
	Matrix_f64(Matrix_f64 && sourceMat); //Move constructor
	~Matrix_f64();

	Matrix_f64(Array2D<float> const & sourceArr);
	Matrix_f64(Array2D<double> const & sourceArr);
	Matrix_f64(Array2D<int> const & sourceArr);
	Matrix_f64(Array2D<long> const & sourceArr);

	Matrix_f64 & operator= (Matrix_f64 const & mat2);
	Matrix_f64 & operator= (Matrix_f64 && mat2);
	Matrix_f64 operator* (const Matrix_f64 & mat2) const;
	Matrix_f64 operator* (const double & scalar) const;
	Matrix_f64 operator+ (const Matrix_f64 & mat2) const;
	Matrix_f64 operator- (const Matrix_f64 & mat2) const;
	Matrix_f64 & operator+= (Matrix_f64 const & mat2);
	Matrix_f64 & operator-= (Matrix_f64 const & mat2);
	Matrix_f64 & operator*= (Matrix_f64 const & mat2); //No inplace multiplication algorithm implemented (is there any?), this is basically * and = op in one.
	Matrix_f64 & operator*= (const double scalar);

	Matrix_f64 Invert() const;
	double Determinant() const;
	void Overlay(const Matrix_f64 mat2, _INDEX rowOffset, _INDEX columnOffset); //Add another Array2D of non-equal size to this Array2D element by element. If the second Array2D is larger, elements outside the boundary will be clipped. rowOffset and columnOffset determine which elements of the first Array2D the first element of the second Array2D will be added to. //TODO vectorize this
	Matrix_f64 ** DecomposeLU() const; //For squared matrices only. Decomposes the matrix into upper and lower triangular matrices, returns as an array of two [pointers to] Array2Ds, the first being the lower decomposition, and the second the upper. Returns NULL if not decomposable.
	Matrix_f64 ** DecomposeLUP() const; //For squared matrices only. Decomposes the matrix into upper and lower triangular matrices with permuation, returns as an array of three [pointers to] Array2Ds, the first being the lower decomposition, and the second the upper, the third is the permuation matrix. Returns NULL if not decomposable.
	bool NearlyEquals(const Matrix_f64 &mat2, double tolerance);
	bool IsSymmetric(double tolerance) const;

	void AddInPlace(Matrix_f64 const & mat2); //for use with += overload, avoids allocating a third matrix
	void SubtractInPlace(Matrix_f64 const & mat2); //for use with -= overload, avoids allocating a third matrix
	virtual void MultiplyWithScalarInPlace(const double scalar);

#ifdef _VECTORIZED_CODE
	void AddInPlaceVectorized(Matrix_f64 const & mat2); //for use with += overload, avoids allocating a third matrix
	void SubtractInPlaceVectorized(Matrix_f64 const & mat2); //for use with -= overload, avoids allocating a third matrix
	void MultiplyWithScalarInPlaceVectorized(const double scalar);
#endif

	//Static methods
	static Matrix_f64 Identity(_INDEX dimension);	//simple function for quick construction of a identity matrix of size: dimension*dimension.
	static bool AreMultipliable(const Matrix_f64 &mat1, const Matrix_f64 &mat2);	//for m1*n1 and m2*n2 matrices, tests that n1 == m2.
	static bool IsInvertible(Matrix_f64 const & mat, bool checkSingular = false);	//Computing the determinant can take ages with current algorithm for large matrices, so the singularity check is optional.
	static bool IsSymmetric(const Matrix_f64 &mat, double tolerance); //For squared matrices only. Returns false for non-squared matrices. tolerance is the (absolute) allowed error in difference between counterpart values, bellow which they are considered equal.
	static bool AreNearlyEquall(const Matrix_f64 &mat1, const Matrix_f64 &mat2, double tolerance);
	//TODO modify LU/LUT decomp to return smartpointers instead.
	static Matrix_f64 ** DecomposeLU(const Matrix_f64 &mat);  //For squared matrices only. Decomposes the matrix into upper and lower triangular matrices, returns as an array of two [pointers to] Array2Ds, the first being the lower decomposition, and the second the upper. Returns NULL if not decomposable. Deleting memory is responcibility of caller.
	static Matrix_f64 ** DecomposeLUP(const Matrix_f64 &mat); //For squared matrices only. Decomposes the matrix into upper and lower triangular matrices with permuation, returns as an array of three [pointers to] Array2Ds, the first being the lower decomposition, and the second the upper, the third is the permuation matrix. Returns NULL if not decomposable. Deleting memory is responcibility of caller.

	static Matrix_f64 AddMatrices(const Matrix_f64 &mat1, const Matrix_f64 &mat2);
	static Matrix_f64 SubtractMatrices(const Matrix_f64 &mat1, const Matrix_f64 &mat2);
	static Matrix_f64 MultiplyMatrices(const Matrix_f64 & mat1, const Matrix_f64 & mat2);
	static Matrix_f64 MultiplayMatrixWithScalar(const Matrix_f64 &mat1, const double scalar);

#ifdef _VECTORIZED_CODE
	static Matrix_f64 AddMatricesVectorized(const Matrix_f64 &mat1, const Matrix_f64 &mat2);
	static Matrix_f64 SubtractMatricesVectorized(const Matrix_f64 &mat1, const Matrix_f64 &mat2);
	//static Matrix_f64 MultiplyMatricesVectorized(const Matrix_f64 & mat1, const Matrix_f64 & mat2); //Not working currently.
	static Matrix_f64 MultiplyMatricesVectorized_N(const Matrix_f64 & mat1, const Matrix_f64 & mat2); //naive implementation that doesn't rely on FMA.
	static Matrix_f64 MultiplayMatrixWithScalarVectorized(const Matrix_f64 &mat1, const double scalar); //Incomplete.
#endif // _VECTORIZED_CODE

	static Matrix_f64 InvertMatrix(const Matrix_f64 & sourceMat, MatrixInversionMethod method = MatrixInversionMethod::Gauss_Jordan);	//switch-statement based on method to use appropriate implementation. Now only Gauss_Jordan, more in future.

	static double CalculateDeterminant(const Matrix_f64 & mat);

protected:
	template <typename T>
	void CopyFromArray2D(Array2D<T> const & sourceArr);

	void AllocateMemory(_INDEX _rows, _INDEX _columns) override;

	//matrix Inversion Methods
	static Matrix_f64 GausJordanElimination(const Matrix_f64 & sourceMat);
	static Matrix_f64 BlockwiseInversion(const Matrix_f64 & sourceMat);

	static Matrix_f64 Invert1x1Matrix(const Matrix_f64 & sourceMat);
	static Matrix_f64 Invert2x2Matrix(const Matrix_f64 & sourceMat);
	static Matrix_f64 Invert3x3Matrix(const Matrix_f64 & sourceMat);
};