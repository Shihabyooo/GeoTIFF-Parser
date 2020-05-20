#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>

//TODO consider having a #define or a typedef at the begining of this header, and switch all instances of size_t to this macro/alias. This would make it easier later for implementers to select
//whether they want to use the large but slow size_t vs the smaller but faster unsigned int/unsigned long int.

enum class MatrixInversionMethod
{
	Gauss_Jordan, //Gaus-Jordan elimination
	//TODO implement more techniques
};

class Array2D
{
private: 
	//Array1D is a helper class to facilitate overloading of double brackets (i.e. foo[][]), since we can only overload operator[].
	//This class should not be exposed to the outside of the Array2D class.
	class Array1D
	{
	public:
		Array1D()
		{
			content_ = NULL;
		};

		Array1D(size_t _columns, double * arrayRow)
		{
			columns_ = _columns;
			content_ = arrayRow;
		};

		~Array1D()
		{
			//Array1D does not own its content, its merely a pointer to a row of the double ** content in Array2D. Deleting Array1D's content means deleting respective row in Array2D's content.
			//leaving this as an empty destructor to prevent default compiler destructors.
		};

		double & operator[] (size_t _column)
		{
			if (content_ == NULL || _column >= columns_)
				throw std::out_of_range("ERROR! Column value out of range or content set to NULL");
			else
				return content_[_column];
		};

	private:
		double * content_ = NULL;
		size_t columns_ = 0;
	};

public:
	//constructors and destructors
	Array2D(size_t _rows, size_t _columns);
	Array2D();
	Array2D(const Array2D & sourceArr); //copy constructor
	Array2D(std::vector<std::vector<double>> & sourceVec); //copy constructor from a vector of vectors of doubles, assumes unequal sub-vectors, allocates for largest one and pads the others with zeroes.
	~Array2D();

	//math overloads
	Array2D operator* (const Array2D & arr2);	//array multiplication overload (with another array)
	Array2D operator* (const double & scalar);	//array multiplication overload (with scalar)
	Array2D operator+ (const Array2D & arr2);	//array additions
	Array2D operator- (const Array2D & arr2);	//arry substraction
	void operator= (const Array2D & sourceArr);	//array assigment from similar type
	void operator= (std::vector<std::vector<double>> & sourceVec);	//array assignment from a vector<vector<double>>, assumes unequal sub-vectors, allocates for largest one and pads the others with zeroes.
	void operator/= (const Array2D & sourceArr);	//array inversion (assigns inverse of the RHS to LHS)
													//TODO consider overloading the / operator to first invert the second array then multiply it with the first array.
	double & operator() (const size_t _row, const size_t _column);	//Array like assignment and reading.
	Array1D & operator[] (const size_t _row)	//This is a multi-step overload of the double bracket operators (foo [][]) using helper object Array1D. Definition has to be in header because Array1D is a a type local to this class.
												//TODO fix: This current implementation is dangerous if a user used foo[] with one set of brackets.
	{
		if (_row >= rows)
			throw std::out_of_range("ERROR! Row value out of range or content set to NULL");

		Array1D currentRow(columns, content[_row]);
		return currentRow;
	};

	//Setters and Getters
	void SetValue(size_t _row, size_t _column, double value);
	double GetValue(size_t _row, size_t _column) const;	//getter, read only.
	size_t Rows() const;	//getter, returns the number of rows of this array, read only.
	size_t Columns() const;	//getter, returns the number of columns of this array, read only.

	//utilities
	void SetEntireArrayToFixedValue(double value);
	Array2D GetSubMatrix(size_t beginRow, size_t noOfRows, size_t beginColumn, size_t noOfColumns);
	Array2D Transpose();	//Returns transpose of this object-array. While it made sense to overload operators for multiplication, addition and inversion, transposing doesn't have a C++ op that we can rationlize equivalence to.
	Array2D Invert();
	double Determinant();
	void SwapRows(size_t firstRow, size_t secondRow);
	bool SwapColumns(size_t firstColumn, size_t secondColumn);	//TODO implement this
	void Overlay(const Array2D &arr2, size_t rowOffset, size_t columnOffset); //Add another Array2D of non-equal size to this Array2D element by element. If the second Array2D is larger, elements outside the boundary will be clipped. rowOffset and columnOffset determine which elements of the first Array2D the first element of the second Array2D will be added to.

	//debugging aid
	void DisplayArrayInCLI(size_t displayPrecision = 4);

	//Static methods
	static Array2D Identity(size_t dimension);	//simple function for quick construction of a identity matrix of size: dimension*dimension.
	static bool AreOfSameSize(const Array2D &arr1, const Array2D &arr2);	//for m1*n1 and m2*n2 matrices, tests that m1 == m2 and n1 == n2.
	static bool AreMultipliable(const Array2D &arr1, const Array2D &arr2);	//for m1*n1 and m2*n2 matrices, tests that n1 == m2.
	static bool IsSquared(const Array2D &arr1); //For m*n matrix, tests that m = n.
	static bool IsInvertible(Array2D arr);	//incomplete, for now tests that m = n for an n*m matrix.
	static bool AreJoinable(const Array2D &arr1, const Array2D &arr2, bool testHorizontally = true);	//tests m1 == m2 or n1 == n2 depending on testHorizontally.
	static Array2D MergeArrays(const Array2D &arr1, const Array2D &arr2);	//Stitches two arrays horizontally, both arrays must be of equal row count.

private:
	//basic maths methods
	Array2D MultiplyArrays(const Array2D & arr1, const Array2D & arr2);
	Array2D MultiplayArrayWithScalar(const Array2D &arr1, const double scalar);
	Array2D AddArrays(const Array2D &arr1, const Array2D &arr2, int opSign = +1);	//opSign is a trick to make only one method for both addition and substraction, +1 = addition, -1 = substraction
	Array2D InvertArray(const Array2D & sourceArr, MatrixInversionMethod method = MatrixInversionMethod::Gauss_Jordan);	//switch-statement based on method to use appropriate implementation. Now only Gauss_Jordan, more in future.
	Array2D TransposeArray(const Array2D & sourceArr);

	double CalculateDeterminant(const Array2D & sourceArr);

	//matrix Inversion Methods
	Array2D GausJordanElimination(const Array2D & sourceArr);

	//private utilities (least privilage principle).
	Array2D GetMinorSubMatrix(const Array2D & sourceArr, size_t _row, size_t _column);
	void DeleteContent();
	
	
	//array contents and parameters
	double ** content = NULL;
	size_t rows = 0;
	size_t columns = 0;
};