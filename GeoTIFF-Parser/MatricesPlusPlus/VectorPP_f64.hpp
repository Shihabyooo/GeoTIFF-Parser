#pragma once
#include "MatrixPP_f64.hpp"
#include "VectorPP_f32.hpp"

class Vector_f64 : public Matrix_f64
{
public:
	Vector_f64();
	Vector_f64(_INDEX const size);
	Vector_f64(_INDEX const size, double defaultValue);
	Vector_f64(Vector_f32 const & sourceVec); //Copy constructor (deep copy)
	Vector_f64(Vector_f64 const & sourceVec); //Copy constructor (deep copy)
	Vector_f64(Vector_f64 && sourceVec); //Move constructor
	Vector_f64(Matrix_f32 const & sourceVec, _INDEX column = 0); //Creates a vector from the column of a matrix.
	Vector_f64(Matrix_f64 const & sourceVec, _INDEX column = 0); //Creates a vector from the column of a matrix.
	~Vector_f64();

	Vector_f64 & operator= (Vector_f32 const & vec2);
	Vector_f64 & operator= (Vector_f64 const & vec2);
	Vector_f64 & operator= (Vector_f64 && vec2);
	Vector_f64 operator+ (Vector_f64 const & vec2) const;
	Vector_f64 operator- (Vector_f64 const & vec2) const;
	Vector_f64 operator* (double const scalar);
	Vector_f64 & operator+= (Vector_f64 const & vec2);
	Vector_f64 & operator-= (Vector_f64 const & vec2);
	Vector_f64 & operator*= (const double scalar);
	double & operator[] (const _INDEX row);
	double operator[] (const _INDEX row) const;
	
	double GetValue(_INDEX row) const;	//getter, read only.
	double Magnitude();
	double DotProduct(Vector_f64 const & vec2) const; //Multiplies the transpose of this vector with vec2

	void AddInPlace(Vector_f64 const & vec2); //for use with += overload, avoids allocating a third vector
	void SubtractInPlace(Vector_f64 const & vec2); //for use with -= overload, avoids allocating a third vector
	void MultiplyWithScalarInPlace(const double scalar) override;

#ifdef _VECTORIZED_CODE
	void AddInPlaceVectorized(Vector_f64 const & vec2); //for use with += overload, avoids allocating a third vector
	void SubtractInPlaceVectorized(Vector_f64 const & vec2); //for use with -= overload, avoids allocating a third vector
	void MultiplyWithScalarInPlaceVectorized(const double scalar);
#endif

	static Vector_f64 AddVectors(Vector_f64 const & vec1, Vector_f64 const & vec2);
	static Vector_f64 SubtractVectors(Vector_f64 const & vec1, Vector_f64 const & vec2);
	static Vector_f64 MultiplayVectorWithScalar(Vector_f64 const & vec1, double const scalar);

#ifdef _VECTORIZED_CODE
	static Vector_f64 AddVectorsVectorized(Vector_f64 const & vec1, Vector_f64 const & vec2);
	static Vector_f64 SubtractVectorsVectorized(Vector_f64 const & vec1, Vector_f64 const & vec2);
	static Vector_f64 MultiplayVectorScalarVectorized(Vector_f64 const & vec1, double const scalar);
#endif

	static double DotProduct(Vector_f64 const & vec1, Vector_f64 const & vec2); //Multiplies the transpose of vec1 with vec2

private:
	//Hide methods inapplicable to vectors.
	using Matrix_f64::Matrix_f64; //hide matrix constructors
	using Matrix_f64::DecomposeLU;
	using Matrix_f64::DecomposeLUP;
	using Matrix_f64::CalculateDeterminant;
	using Matrix_f64::IsInvertible;
	using Matrix_f64::IsSymmetric;
	using Matrix_f64::InvertMatrix;
	using Array2D::GetValue;

	//Vector-specific methods
	void VectorFromMatrix(Matrix_f64 const & sourceVec, _INDEX column);
};