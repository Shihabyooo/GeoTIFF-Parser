#pragma once
#include <iostream>
#include <iomanip>
#include <vector>

#define UNMULT_MATRIX 15
#define UNEQUAL_MATRIX 16
#define UNMERGABLE_MATRIX 17 
#define NONINVERT_MATRIX 18
#define NONSQUARE_MATRIX 19

enum MatrixInversionMethod
{
	Gauss_Jordan, //Gaus-Jordan elimination
	//TODO implement more techniques
};

class Array2D
{
private: 
	//Array1D is a helper class to facilitate overloading of double brackets (i.e. foo[][]), since we can only overload operator[].
	//This class must not be exposed to the outside of the Array2D class.
	class Array1D
	{
	public:
		Array1D()
		{
		};

		Array1D(const int _columns)
		{
			content = new double[_columns];
		};

		~Array1D()
		{
			//Array1D does not own its content, its merely a pointer to a row of the double ** content in Array2D. Deleting Array1D's content means deleting respective row in Array2D's content.
			//leaving this as an empty destructor to prevent default compiler destructors.
		};

		void operator= (double * arrayRow)
		{
			content = arrayRow;
		};

		double & operator[] (const int _column)
		{
			return content[_column];
		};

	private:
		double * content;
	};

public:
	//constructors and destructors
	Array2D(int _rows, int _columns);
	Array2D();
	Array2D(const Array2D & sourceArr); //copy constructor
	Array2D(std::vector<std::vector<double>> & sourceVec); //copy constructor from a vector of vectors of doubles, assumes unequal sub-vectors, allocates for largest one and pads the others with zeroes.
	~Array2D();

	//math overloads
	Array2D operator* (const Array2D & arr2);	//array multiplication overload (with another array)
	Array2D operator* (const double & scalar);	//array multiplication overload (with scalar)
	Array2D operator+ (const Array2D & arr2);	//arry additions
	Array2D operator- (const Array2D & arr2);	//arry substraction
	void operator= (const Array2D & sourceArr);	//array assigment from similar type
	void operator= (std::vector<std::vector<double>> & sourceVec);	//array assignment from a vector<vector<double>>, assumes unequal sub-vectors, allocates for largest one and pads the others with zeroes.
	void operator/= (const Array2D & sourceArr);	//array inversion (assigns inverse of the RHS to LHS)
													//TODO consider overloading the / operator to first invert the second array then multiply it with the first array.
	double & operator() (const int _row, const int _column);	//Array like assignment and reading.
	Array1D & operator[] (const int _row)	//This is a multi-step overload of the double bracket operators (foo [][]) using helper object Array1D. Definition has to be in header because Array1D is a a type local to this class.
	{
		Array1D currentRow(columns);
		currentRow = content[_row];
		return currentRow;
	};

	//Setters and Getters
	bool SetValue(int _row, int _column, double value);
	double GetValue(int _row, int _column) const;	//getter, read only.
	int Rows() const;	//getter, returns the number of rows of this array, read only.
	int Columns() const;	//getter, returns the number of columns of this array, read only.

	//utilities
	void SetEntireArrayToFixedValue(double value);
	Array2D GetSubMatrix(unsigned int beginRow, unsigned int noOfRows, unsigned int beginColumn, unsigned int noOfColumns);
	Array2D Transpose();	//Returns transpose of this object-array. While it made sense to overload operators for multiplication, addition and inversion, transposing doesn't have a C++ op that we can rationlize equivalence to.
	Array2D Invert();
	double Determinant();
	bool SwapRows(unsigned int firstRow, unsigned int secondRow);
	bool SwapColumns(int firstColumn, int secondColumn);	//TODO implement this
	void Overlay(const Array2D &arr2, int rowOffset, int columnOffset); //Add another Array2D of non-equal size to this Array2D element by element. If the second Array2D is larger, elements outside the boundary will be clipped. rowOffset and columnOffset determine which elements of the first Array2D the first element of the second Array2D will be added to.

	//debugging aid
	void DisplayArrayInCLI(int displayPrecision = 4);

	//Static methods
	static Array2D Identity(int dimension);	//simple function for quick construction of a identity matrix of size: dimension*dimension.
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
	Array2D GetMinorSubMatrix(const Array2D & sourceArr, unsigned int _row, unsigned int _column);
	void DeleteContent();
	
	
	//array contents and parameters
	double ** content;
	int rows, columns;
};