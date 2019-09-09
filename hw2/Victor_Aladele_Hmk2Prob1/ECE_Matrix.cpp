/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 6, 2019
Description:
 Implementing matrix class for several matrix operations
*/

#include "ECE_Matrix.h"

// default constructor
ECE_Matrix::ECE_Matrix()
{
    nRows = 0; 
    nCols = 0;
}

// copy constructor
ECE_Matrix::ECE_Matrix(const ECE_Matrix &matrix_obj)
{
    nRows = matrix_obj.nRows;
    nCols = matrix_obj.nCols;
    matrix.resize(nRows);
    for (int i = 0; i < matrix_obj.nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < matrix_obj.nCols; j++)
        {
            // Copy over everything
            matrix[i][j] = matrix_obj.matrix[i][j];
        }
    }
}
// Construct matDim x matDim array set all elements to matEntry
ECE_Matrix::ECE_Matrix(int matDim, double matEntry)
{
    nRows = matDim;
    nCols = matDim;
    matrix.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            matrix[i][j] = matEntry; // set matrix entries
        }
    }
}

// Construct nRowIn x nColIn array set all elements to matEntry
ECE_Matrix::ECE_Matrix(int nRowsIn, int nColsIn, double matEntry)
{
    nRows = nRowsIn;
    nCols = nColsIn;
    matrix.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            matrix[i][j] = matEntry; // set matrix entries
        }
    }
}

// Construct matrix from file
ECE_Matrix::ECE_Matrix(std::string input_file)
{
    std::ifstream data_matrix;
    data_matrix.open(input_file); // open text file
    data_matrix >> nRows >> nCols;

    matrix.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            data_matrix >> matrix[i][j]; // read from text file
        }
    }
    data_matrix.close(); // close file
}

// print out matrix by overloading the << operator
std::ostream& operator<<(std::ostream& oss, const ECE_Matrix &matrix_obj)
{
    for (int i = 0; i < matrix_obj.nRows; i++)
    {
        for (int j = 0; j < matrix_obj.nCols; j++)
        {
            oss << std::scientific << std::setw(12) << std::setprecision(3) << matrix_obj.matrix[i][j]; // add the matrix to oss in the right format
        }
        oss << std::endl;
    }
    return oss;
}

// assign matrix to another matrix
ECE_Matrix& ECE_Matrix::operator=(const ECE_Matrix &matrix_obj)
{
    // Check to see if we are trying to assign ourself
    if (this == &matrix_obj)
    {
       return *this;
    }
    nRows = matrix_obj.nRows;
    nCols = matrix_obj.nCols;
    matrix.resize(nRows);
    for (int i = 0; i < matrix_obj.nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < matrix_obj.nCols; j++)
        {
            // Copy over everything
            matrix[i][j] = matrix_obj.matrix[i][j];
        }
    }
    // Return
    return *this;
}

// subtract a scalar from matrix (scalar - matrix)
ECE_Matrix operator-(const double &numIn, const ECE_Matrix &matrix_obj) 
{
    ECE_Matrix newECE_Matrix(matrix_obj);
    return (newECE_Matrix - numIn);
}

// subtract a scalar from a matrix (matrix - scalar)
ECE_Matrix ECE_Matrix::operator-(const double &numIn) const
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Decrement each matrix entry by numIn
            newECE_Matrix.matrix[i][j] -= numIn;
        }
    }
    return newECE_Matrix;
}

// reshape matrix and prep for addition or subtraction of matrices of different shapes
ECE_Matrix ECE_Matrix::reshape_mat(int nRowIn, int nColIn)
{
    ECE_Matrix modECE_Matrix(nRowIn, nColIn, 0);
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Add the original matrix to the zero matrix
            modECE_Matrix.matrix[i][j] += matrix[i][j];
        }
    }
    return modECE_Matrix;
}

// add two matrix objects together
ECE_Matrix ECE_Matrix::operator+(const ECE_Matrix &matrix_obj) const
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    ECE_Matrix newMatrix_obj(matrix_obj);
    newECE_Matrix = newECE_Matrix.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols)); // reshape the matrix for addition op
    newMatrix_obj = newMatrix_obj.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols)); // reshape the matrix for addition op
    for (int i = 0; i < newECE_Matrix.nRows; i++)
    {
        for (int j = 0; j < newECE_Matrix.nCols; j++)
        {
            // Add each matrix entry to the other. 
            newECE_Matrix.matrix[i][j] += newMatrix_obj.matrix[i][j];
        }
    }
    return newECE_Matrix;
}

// subtract one matrix object from another matrix object 
ECE_Matrix ECE_Matrix::operator-(const ECE_Matrix &matrix_obj) const
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    ECE_Matrix newMatrix_obj(matrix_obj);
    newECE_Matrix = newECE_Matrix.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols));
    newMatrix_obj = newMatrix_obj.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols));
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Add each matrix entry to the other. 
            newECE_Matrix.matrix[i][j] -= newMatrix_obj.matrix[i][j];
        }
    }
    return newECE_Matrix;
}

// add matrix to a scalar (matrix_obj + scalar)
ECE_Matrix ECE_Matrix::operator+(const double &numIn) const
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Increment each matrix entry by numIn
            newECE_Matrix.matrix[i][j] += numIn;
        }
    }
    return newECE_Matrix;
}

// add a scalar to a matrix (scalar + matrix)
ECE_Matrix operator+(const double &numIn, const ECE_Matrix &matrix_obj) 
{
    ECE_Matrix newECE_Matrix(matrix_obj);
    return (newECE_Matrix + numIn);
}

// multiply a matrix by a scalar (scalar * matrix_obj)
ECE_Matrix operator*(const double &numIn, const ECE_Matrix &matrix_obj) 
{
    ECE_Matrix newECE_Matrix(matrix_obj);
    return (newECE_Matrix * numIn);
}

// return transpose of matrix
ECE_Matrix ECE_Matrix::transpose() const
{
    ECE_Matrix newECE_Matrix(nCols, nRows, 0); // create a matrix_obj with matrix dimensions (nCols, nRows), and set all matrix entries to zero
    for (int i = 0; i < nCols; i++)
    {
        for (int j = 0; j < nRows; j++)
        {
            // Add the original matrix to the zero matrix
            newECE_Matrix.matrix[i][j] = matrix[j][i];
        }
    }
    return newECE_Matrix;
}

// multiply a matrix by a scalar (matrix_obj * scalar)
ECE_Matrix ECE_Matrix::operator*(const double &numIn) const
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Increment each matrix entry by numIn
            newECE_Matrix.matrix[i][j] *= numIn;
        }
    }
    return newECE_Matrix;
}

// divide a matrix by a scalar (matrix_obj / scalar)
ECE_Matrix ECE_Matrix::operator/(const double &numIn) const
{
    if (numIn == 0)
    {
        ECE_Matrix newECE_Matrix(nCols, nRows, 0); // create a matrix_obj with matrix dimensions (nCols, nRows), and set all matrix entries to zero
        return newECE_Matrix;
    }

    ECE_Matrix newECE_Matrix(*this);
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            // Increment each matrix entry by numIn
            newECE_Matrix.matrix[i][j] /= numIn;
        }
    }
    return newECE_Matrix;
}

// incrementing matrix by adding values of another matrix
ECE_Matrix& ECE_Matrix::operator+=(const ECE_Matrix &matrix_obj)
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    ECE_Matrix newMatrix_obj(matrix_obj);
    newECE_Matrix = newECE_Matrix.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols));
    newMatrix_obj = newMatrix_obj.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols)); // reshape the matrix for addition op

    newECE_Matrix = newECE_Matrix + matrix_obj;

    nRows = std::max(nRows, matrix_obj.nRows);
    nCols = std::max(nCols, matrix_obj.nCols);
    matrix.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {   
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            // Increment each matrix entry by numIn
            matrix[i][j] = newECE_Matrix.matrix[i][j];
        }
    }
    return *this;
}

// decrementing matrix by adding values of another matrix
ECE_Matrix& ECE_Matrix::operator-=(const ECE_Matrix &matrix_obj)
{
    ECE_Matrix newECE_Matrix(*this); // create a copy of the lhs operand
    ECE_Matrix newMatrix_obj(matrix_obj);
    newECE_Matrix = newECE_Matrix.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols));
    newMatrix_obj = newMatrix_obj.reshape_mat(std::max(nRows, matrix_obj.nRows), std::max(nCols, matrix_obj.nCols)); // reshape the matrix for addition op

    newECE_Matrix = newECE_Matrix - matrix_obj;

    nRows = std::max(nRows, matrix_obj.nRows);
    nCols = std::max(nCols, matrix_obj.nCols);
    matrix.resize(nRows);
    for (int i = 0; i < nRows; i++)
    {   
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            // Increment each matrix entry by numIn
            matrix[i][j] = newECE_Matrix.matrix[i][j];
        }
    }
    return *this;
}