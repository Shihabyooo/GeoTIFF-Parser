#pragma once
#include "MatrixPP_f32.hpp"

class Vector_f32 : public Matrix_f32
{
public:
	Vector_f32();
	Vector_f32(_INDEX const size);
	Vector_f32(_INDEX const size, float defaultValue);
	Vector_f32(Vector_f32 const & sourceVec); //Copy constructor (deep copy)
	Vector_f32(Vector_f32 && sourceVec); //Move constructor
	//explicit Vector_f32(Matrix_f32 const & sourceVec, _INDEX column = 0); //Creates a vector from the column of a matrix.
	Vector_f32(Matrix_f32 const & sourceVec, _INDEX column = 0); //Creates a vector from the column of a matrix.
	~Vector_f32();

	Vector_f32 & operator= (Vector_f32 const & vec2);
	Vector_f32 & operator= (Vector_f32 && vec2);
	Vector_f32 operator+ (Vector_f32 const & vec2) const;
	Vector_f32 operator- (Vector_f32 const & vec2) const;
	Vector_f32 operator* (double const scalar);
	Vector_f32 & operator+= (Vector_f32 const & vec2);
	Vector_f32 & operator-= (Vector_f32 const & vec2);
	Vector_f32 & operator*= (const double scalar);
	float & operator[] (const _INDEX row);
	float operator[] (const _INDEX row) const;
	
	float GetValue(_INDEX row) const;	//getter, read only.
	double Magnitude();
	double DotProduct(Vector_f32 const & vec2) const; //Multiplies the transpose of this vector with vec2
	
	void AddInPlace(Vector_f32 const & vec2); //for use with += overload, avoids allocating a third vector
	void SubtractInPlace(Vector_f32 const & vec2); //for use with -= overload, avoids allocating a third vector
	void MultiplyWithScalarInPlace(const double scalar) override;

#ifdef _VECTORIZED_CODE
	void AddInPlaceVectorized(Vector_f32 const & vec2); //for use with += overload, avoids allocating a third vector
	void SubtractInPlaceVectorized(Vector_f32 const & vec2); //for use with -= overload, avoids allocating a third vector
	void MultiplyWithScalarInPlaceVectorized(const double scalar);
#endif

	static Vector_f32 AddVectors(Vector_f32 const & vec1, Vector_f32 const & vec2);
	static Vector_f32 SubtractVectors(Vector_f32 const & vec1, Vector_f32 const & vec2);
	static Vector_f32 MultiplayVectorWithScalar(Vector_f32 const & vec1, double const scalar);

#ifdef _VECTORIZED_CODE
	static Vector_f32 AddVectorsVectorized(Vector_f32 const & vec1, Vector_f32 const & vec2);
	static Vector_f32 SubtractVectorsVectorized(Vector_f32 const & vec1, Vector_f32 const & vec2);
	static Vector_f32 MultiplayVectorScalarVectorized(Vector_f32 const & vec1, double const scalar);
#endif

	static double DotProduct(Vector_f32 const & vec1, Vector_f32 const & vec2); //Multiplies the transpose of vec1 with vec2

private:
	//Hide methods inapplicable to vectors.
	using Matrix_f32::Matrix_f32; //hide matrix constructors
	using Matrix_f32::DecomposeLU;
	using Matrix_f32::DecomposeLUP;
	using Matrix_f32::CalculateDeterminant;
	using Matrix_f32::IsInvertible;
	using Matrix_f32::IsSymmetric;
	using Matrix_f32::InvertMatrix;
	using Array2D::GetValue;

	//Vector-specific methods
	void VectorFromMatrix(Matrix_f32 const & sourceVec, _INDEX column);
};