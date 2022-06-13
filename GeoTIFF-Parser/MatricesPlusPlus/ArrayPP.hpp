#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <stdexcept>
#include <cmath>

//uncomment the line bellow to have every method that reads/writes content into the array first check that the supplied index is within range.
//#define _USE_BOUNDS_CHECK

//TODO define out of bounds check macro or add a method in the class to do so, replace instances in code with it.

//can replace size_t with an integer type (e.g. int or unsigned short). Performance gains to be tested
#define _INDEX size_t

//MINDET is the value bellow which the absolute of the determinant is considered zero (i.e. matrix would be considered singular)
#define MINDET 0.001F 


#define Alloc(x, y) try\
	{\
	AllocateMemory(x, y);\
	}\
	catch (const std::bad_alloc& exception)\
	{\
		throw exception;\
	}

enum class MatrixInversionMethod
{
	Gauss_Jordan, //Gaus-Jordan elimination
	Blockwise,
	//TODO implement more techniques
};

template <typename T>
class Array2D
{
public:
	//constructors and destructors
	Array2D();
	Array2D(_INDEX _rows, _INDEX _columns);
	Array2D(_INDEX _rows, _INDEX _columns, T defaultValue);
	Array2D(const Array2D<T> & sourceArr); //copy constructor (Deep copy)
	//Array2D(std::vector<std::vector<T>> & sourceVec); //copy constructor from a vector of vectors of T, assumes unequal sub-vectors, allocates for largest one and pads the others with default value for T. (Deep copy)
	~Array2D();

	//operator overloads
	void operator= (const Array2D<T> & sourceArr);	//array assigment from similar type, performs deep copy of the RHS Array2D content
	//void operator= (std::vector<std::vector<T>> & sourceVec);	//array assignment from a vector<vector<T>>, assumes unequal sub-vectors, allocates for largest one and pads the others with default value for T.
	bool operator== (const Array2D<T> arr2); //equality check
	bool operator!= (const Array2D<T> arr2); //non equality check (what?)
	T & operator() (const _INDEX _row, const _INDEX _column);	//Array like assignment and reading. //TODO force type matching
	T * const & operator[] (const _INDEX _row);

	//Setters and Getters
	void SetValue(_INDEX _row, _INDEX _column, T value);
	T GetValue(_INDEX _row, _INDEX _column) const;	//getter, read only.
	_INDEX Rows() const;	//getter, returns the number of rows of this array, read only.
	_INDEX Columns() const;	//getter, returns the number of columns of this array, read only.
	bool IsEmpty() const;

	//utilities
	void SetEntireArrayToFixedValue(T value);
	Array2D<T> GetSubMatrix(_INDEX beginRow, _INDEX noOfRows, _INDEX beginColumn, _INDEX noOfColumns) const;
	Array2D<T> Transpose();	//Returns transpose of this object-array. While it made sense to overload operators for multiplication, addition and inversion, transposing doesn't have a C++ op that we can rationlize equivalence to.
	void SwapRows(_INDEX firstRow, _INDEX secondRow);
	//void Overlay(const Array2D<T> &arr2, _INDEX rowOffset, _INDEX columnOffset); //Add another Array2D of non-equal size to this Array2D element by element. If the second Array2D is larger, elements outside the boundary will be clipped. rowOffset and columnOffset determine which elements of the first Array2D the first element of the second Array2D will be added to.
	bool IsSymmetric();

	//debugging aid
	void DisplayArrayInCLI(unsigned int displayPrecision = 4);

	//Static methods
	static bool AreOfSameSize(const Array2D<T> &arr1, const Array2D<T> &arr2);	//for m1*n1 and m2*n2 matrices, tests that m1 == m2 and n1 == n2.
	static bool IsSquared(const Array2D<T> &arr1); //For m*n matrix, tests that m = n.
	static bool IsSymmetric(const Array2D<T> &arr); //For squared matrices only. Returns false for non-squared matrices. Tests direct equality (depends on type T).
	//static bool IsSymmetric(const Array2D<T> &arr, float tolerance); //For squared matrices only. Returns false for non-squared matrices. tolerance is the (absolute) allowed error in difference between counterpart values, bellow which they are considered equal.
	static bool AreJoinable(const Array2D<T> &arr1, const Array2D<T> &arr2, bool testHorizontally = true);	//tests m1 == m2 or n1 == n2 depending on testHorizontally.
	static Array2D<T> MergeArrays(const Array2D<T> &arr1, const Array2D<T> &arr2);	//Stitches two arrays horizontally, both arrays must be of equal row count.
	static Array2D<T> StackArrays(const Array2D<T> &arr1, const Array2D<T> &arr2);
protected:
	//basic methods
	Array2D<T> TransposeArray(const Array2D<T> & sourceArr);
	static Array2D<T> GetMinorSubMatrix(const Array2D<T> & sourceArr, _INDEX _row, _INDEX _column);
	
	//private utilities
	virtual void AllocateMemory(_INDEX _rows, _INDEX _columns);
	void DeleteContent();
	
	//array contents and parameters
	T ** content = NULL;
	_INDEX rows = 0;
	_INDEX columns = 0;
};