#include "ArrayPP.hpp"

template <typename T>
Array2D<T>::Array2D()
{
	rows = 0;
	columns = 0;
	content = NULL;
}

template <typename T>
Array2D<T>::Array2D(_INDEX _rows, _INDEX _columns)
{
	Alloc(_rows, _columns);

	columns = _columns;
	rows = _rows;
}

template <typename T>
Array2D<T>::Array2D(_INDEX _rows, _INDEX _columns, T defaultValue)
{
	Alloc(_rows, _columns);

	columns = _columns;
	rows = _rows;

	SetEntireArrayToFixedValue(defaultValue);
}

template <typename T>
Array2D<T>::Array2D(const Array2D<T> & sourceArr)
{
	//content = NULL;
	*this = sourceArr;
}

// template <typename T>
// Array2D<T>::Array2D(std::vector<std::vector<T>> & sourceVec)
// {
// 	//content = NULL;
// 	*this = sourceVec;
// }

template <typename T>
Array2D<T>::~Array2D()
{
	DeleteContent();
}

template <typename T>
void Array2D<T>::operator=(const Array2D<T> & sourceArr)
{
	//Before assigning a new conent to current instance of an object, we must first delete its current content, if it exists. The existence check is already done inside DeleteContent().
	DeleteContent();

	rows = sourceArr.Rows();
	columns = sourceArr.Columns();

	//In this implementation, we accept that we could assign an "empty" object to this one. We set content = NULL because that's how we establish this object to be empty.
	if (sourceArr.content == NULL)
	{
		content = NULL;
		return;
	}

	Alloc(rows, columns);

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < columns; j++)
			content[i][j] = sourceArr.GetValue(i, j);
}

// template <typename T>
// void Array2D<T>::operator=(std::vector<std::vector<T>>& sourceVec)
// {
// 	//The assignment from a vector of vectors of doubles has a worst-case-scenario assumption that not all of the secondary vectors are of same length (no guarantees of that), so, we
// 	//create a matrix of number of rows equal to length of first vector, and of columns equal to the largest the sub-vectors, we copy values from the vector<vector> array as expected,
// 	//but we pad the cells in shorter rows with zeroes. E.g. for vector of 3 sub-vectors, where sub-vector1 = {1, 2, 3}, sub-vector2 = {1, 2}, and sub-vector3 = {1, 2, 3, 4}, the result
// 	//matrix is:
// 	//	1	2	3	0
// 	//	1	2	0	0
// 	//	1	2	3	4

// 	//Before assigning a new conent to current instance of an object, we must first delete its current content, if it exists. The existence check is already done inside DeleteContent().
// 	DeleteContent();

// 	//First, we determine the number of columns = largest row size (longest sub-vector)
// 	_INDEX maxRowSize = 0;

// 	for (std::vector<std::vector<T>>::iterator it = sourceVec.begin(); it != sourceVec.end(); ++it)
//         maxRowSize = maxRowSize > it->size() ? maxRowSize : it->size();
	
// 	rows = sourceVec.size();
// 	columns = maxRowSize;

// 	if (rows == 0 || columns == 0) //In case the Vector matrix we're working with is empty.
// 	{
// 		content = NULL;
// 		return;
// 	}

// 	Alloc(rows, columns);

// 	_INDEX currentRow = 0, currentColumn = 0;

// 	for (std::vector<std::vector<T>>::iterator it = sourceVec.begin(); it != sourceVec.end(); ++it)
// 	{
// 		for (std::vector<double>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
// 		{
// 			content[currentRow][currentColumn] = *it2;
// 			currentColumn++;
// 		}
// 		currentColumn = 0;
// 		currentRow++;
// 	}
// }

template <typename T>
bool Array2D<T>::operator==(const Array2D<T> arr2)
{
	if (rows != arr2.Rows() || columns != arr2.Columns())
		return false;

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < columns; j++)
		{
			if (content[i][j] != arr2.GetValue(i, j))
				return false;
		}
	}
	return true;
}

template <typename T>
bool Array2D<T>::operator!=(const Array2D<T> arr2)
{
	return !(*this == arr2);
}

template <typename T>
T & Array2D<T>::operator()(_INDEX _row, _INDEX _column)
{
	return content[_row][_column];
}

template <typename T>
T * const & Array2D<T>::operator[] (const _INDEX _row)
{
	#ifdef _USE_BOUNDS_CHECK
	if (content == NULL			//Checking whether this object is empty. Making an assumption that initializing the first level of the content is automatically followed by init of sublevel.
		|| _row >= rows)		//Checking out-of-bound writes.
		throw std::out_of_range("ERROR! Column or Row value out of range or content set to NULL");
    #endif

	return content[_row];
}

template <typename T>
void Array2D<T>::SetValue(_INDEX _row, _INDEX _column, T value)
{
    #ifdef _USE_BOUNDS_CHECK
	if (content == NULL							//Checking whether this object is empty. Making an assumption that initializing the first level of the content is automatically followed by init of sublevel.
		|| _row >= rows || _column >= columns)	//Checking out-of-bound writes.
		throw std::out_of_range("ERROR! Column or Row value out of range or content set to NULL");
    #endif

	content[_row][_column] = value;
}

template <typename T>
T Array2D<T>::GetValue(_INDEX _row, _INDEX _column) const
{
    #ifdef _USE_BOUNDS_CHECK
	if (content == NULL							//Checking whether this object is empty. Making an assumption that initializing the first level of the content is automatically followed by init of sublevel.
		|| _row >= rows || _column >= columns)	//Checking out-of-bound writes.
		throw std::out_of_range("ERROR! Column or Row value out of range or content set to NULL");
    #endif

	return content[_row][_column];
}

template <typename T>
_INDEX Array2D<T>::Rows() const
{
	return rows;
}

template <typename T>
_INDEX Array2D<T>::Columns() const
{
	return columns;
}

template <typename T>
bool Array2D<T>::IsEmpty() const
{
	if(content == NULL || (rows < 1 && columns < 1))
		return true;

	return false;
}

template <typename T>
void Array2D<T>::SetEntireArrayToFixedValue(T value)
{
	for (_INDEX i = 0; i < rows; i++)
	{
		for (_INDEX j = 0; j < columns; j++)
		{
			content[i][j] = value;
		}
	}
}

template <typename T>
Array2D<T> Array2D<T>::GetSubMatrix(_INDEX beginRow, _INDEX noOfRows, _INDEX beginColumn, _INDEX noOfColumns) const
{
	
    #ifdef _USE_BOUNDS_CHECK
	if (content == NULL || beginRow + noOfRows > rows || beginColumn + noOfColumns > columns)  //TODO consider rearranging the arguments for this method to have the pivot coord
																			//first then lengths.
	{
		//std::cout << "ERROR! Attempting to extract submatrix with a range outside the original matrix bounds" << std::endl;
		throw std::out_of_range("ERROR! Column or Row value out of range or content set to NULL");
	}
    #endif

	if (noOfRows < 1 || noOfColumns < 1)
	{
		std::cout << "ERROR! Attempting to extract submatrix with null size in one both dimensions." << std::endl;
		return Array2D<T>();
	}
    
	Array2D<T> subMatrix(noOfRows, noOfColumns);

	for (_INDEX i = beginRow; i < beginRow + noOfRows; i++)
	{
		for (_INDEX j = beginColumn; j < beginColumn + noOfColumns; j++)
		{
			subMatrix.SetValue(i - beginRow, j - beginColumn, content[i][j]);
		}
	}

	return subMatrix;
}

template <typename T>
Array2D<T> Array2D<T>::Transpose()
{
	return TransposeArray(*this);
}

template <typename T>
void Array2D<T>::SwapRows(_INDEX firstRow, _INDEX secondRow)
{
    #ifdef _USE_BOUNDS_CHECK
	//Check whether firstRow or secondRow are OOB, return false if so.
	if (firstRow > rows || secondRow > rows)
	{
		//std::cout << "ERROR! Attempting to swap rows out of array's bounds." << std::endl;
		throw std::out_of_range("ERROR! Attempting to swap columns outside the range of the array");
	}
    #endif

	for (_INDEX j = 0; j < columns; j++)
	{
		T tempValue = content[secondRow][j]; //backup second row value
		content[secondRow][j] = content[firstRow][j];
		content[firstRow][j] = tempValue;
	}
}

template <typename T>
bool Array2D<T>::IsSymmetric()
{
	return IsSymmetric(*this);
}

template <typename T>
bool Array2D<T>::AreOfSameSize(const Array2D<T> & arr1, const Array2D<T> & arr2)
{
	if (arr1.Rows() != arr2.Rows() || arr1.Columns() != arr2.Columns())
		return false;
	else
		return true;
}

template <typename T>
bool Array2D<T>::IsSquared(const Array2D<T> & arr1)
{
	return arr1.Rows() == arr1.Columns();
}

template <typename T>
bool Array2D<T>::IsSymmetric(const Array2D<T> & arr)
{
	if (!IsSquared(arr))
		return false;

	for (_INDEX i = 0; i < arr.Rows(); i++)
	{
		for (_INDEX j = 0; j < arr.Rows(); j++)
		{
			if (arr.GetValue(i, j) != arr.GetValue(j, i))
				return false;
		}
	}

	return true;
}

template <typename T>
bool Array2D<T>::AreJoinable(const Array2D<T> & arr1, const Array2D<T> & arr2, bool testHorizontally)
{
	if (testHorizontally) //testing for horizontal merging, row count must be equall.
	{
		if (arr1.Rows() == arr2.Rows())
			return true;
		else
			return false;
		
	}
	else //testing for vertical merging, column count must be equall.
	{
		if (arr1.Columns() == arr2.Columns())
			return true;
		else
			return false;
	}
}

template <typename T>
Array2D<T> Array2D<T>::MergeArrays(const Array2D<T> & arr1, const Array2D<T> & arr2)
{
	if (!AreJoinable(arr1, arr2, true))
	{
		std::cout << "ERROR! Attempting to merge array of different heights" << std::endl;
		return Array2D();
	}

	Array2D result(arr1.Rows(), arr1.Columns() + arr2.Columns());

	for (_INDEX row = 0; row < result.Rows(); row++)
	{
		for (_INDEX column = 0; column < arr1.Columns(); column++) //loop over the western (left) half of the array.
			result.SetValue(row, column, arr1.GetValue(row, column));

		for (_INDEX column = arr1.Columns(); column < arr1.Columns() + arr2.Columns(); column++) //loop over the easter (right) half of the array.
			result.SetValue(row, column, arr2.GetValue(row, column - arr1.Columns()));
	}

	return result;
}

template <typename T>
Array2D<T> Array2D<T>::StackArrays(const Array2D<T> &arr1, const Array2D<T> &arr2)
{
	if (!AreJoinable(arr1, arr2, false))
	{
		std::cout << "ERROR! Attempting to stack array of different widths" << std::endl;
		return Array2D();
	}

	Array2D result(arr1.Rows() + arr2.Rows(), arr1.Columns());

	for (_INDEX column = 0; column < arr1.Columns(); column++ )
	{
		//loop over northern half
		for (_INDEX row = 0; row < arr1.Rows(); row++ )
			result[row][column] = arr1.GetValue(row, column);

		for (_INDEX row = arr1.Rows(); row < arr1.Rows() + arr2.Rows(); row++ )
			result[row][column] = arr2.GetValue(row - arr1.Rows(), column);
	}

	return result;
}

template <typename T>
void Array2D<T>::DisplayArrayInCLI(unsigned int displayPrecision)
{
	if (content == NULL)
	{
		std::cout << "WARNING! Array content not initialized.\nCannnot display array content." << std::endl;
		return;
	}

	std::cout << "- - - - - - - - - - - - - -" << std::endl;
	for (_INDEX i = 0; i < rows; i++)
	{
		for (_INDEX j = 0; j < columns; j++)
		{
			std::cout << std::fixed << std::setw(displayPrecision + 4) << std::setprecision(displayPrecision) << content[i][j] << "\t";
		}
		std::cout << std::endl;
	}
	std::cout << "- - - - - - - - - - - - - -" << std::endl;
}

template <typename T>
Array2D<T> Array2D<T>::TransposeArray(const Array2D<T> & sourceArr)
{
	if (sourceArr.content == NULL)
	{
		std::cout << "ERROR! Attempting to transpose a non-initialized Array2D" <<std::endl;
		return Array2D();
	}

	Array2D<T> result (sourceArr.Columns(), sourceArr.Rows());

	for (_INDEX i = 0; i < result.Rows(); i++)
	{
		for (_INDEX j = 0; j < result.Columns(); j++)
		{
			result.SetValue(i, j, sourceArr.GetValue(j, i));
		}
	}

	return result;
}

template <typename T>
Array2D<T> Array2D<T>::GetMinorSubMatrix(const Array2D<T> & sourceArr, _INDEX _row, _INDEX _column)
{
	//The ij-minor sub-matrix is obtained by deleting the ith row and jth column of a matrix.

	Array2D<T> result(sourceArr.Rows() - 1, sourceArr.Columns() - 1);

	for (_INDEX i = 0; i < result.Rows(); i++) //this loop iterates on the smaller, sub matrix. We maintain seperate indeces inside for use when reading from source matrix.
	{
		_INDEX rowInSourceArr = i;
		if (i >= _row) //This means we've reached the row we want to ommit, so now we read the next row.
			rowInSourceArr += 1;

		for (_INDEX j = 0; j < result.Columns(); j++)
		{
			_INDEX columnInSourceArr = j;
			if (j >= _column) //This means we've reached the column we want to ommit, so now we read the next column.
				columnInSourceArr += 1;

			result.SetValue(i, j, sourceArr.GetValue(rowInSourceArr, columnInSourceArr));
		}
	}

	return result;
}

template <typename T>
void Array2D<T>::AllocateMemory(_INDEX _rows, _INDEX _columns)
{
	try
	{
		content = new T*[_rows];
		T * helperPtr = new T[_rows * _columns]();

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

template <typename T>
void Array2D<T>::DeleteContent()
{
	if (content != NULL)
	{
		if (content[0] != NULL)
			delete[] content[0];

		delete[] content;
		content = NULL;
	}
	columns = 0;
	rows = 0;
}