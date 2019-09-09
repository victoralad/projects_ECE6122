/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 6, 2019
Description:
 Implementing matrix class for several matrix operations
*/

#ifndef ECE_MATRIX_H
#define ECE_MATRIX_H

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

// class to perform several matrix operations
class ECE_Matrix
{
    int nRows, nCols; // declare the dimensions of the matrix
    std::vector<std::vector<double>> matrix; // declare the matrix as a two dimensional vector
public:
    ECE_Matrix(const ECE_Matrix &matrix_obj); // copy constructor (compiler has the ability to implicitly create this copy constructor if needed)
    ECE_Matrix (int matDim, double matEntry); // Construct matDim x matDim array set all elements to matEntry
    ECE_Matrix (int nRowIn, int nColIn, double matEntry); // Construct nRowIn x nColIn array set all elements to matEntry
    ECE_Matrix (std::string input_file); // Construct matrix from file
    friend std::ostream& operator<<(std::ostream &osstream, const ECE_Matrix &matrix_obj); // print out matrix by overloading the << operator
    ECE_Matrix& operator=(const ECE_Matrix &matrix_obj); // assign matrix to another matrix
    ECE_Matrix operator-(const double &numIn) const; // subtract a scalar from a matrix (matrix_obj - scalar)
    ECE_Matrix reshape_mat(int nRowIn, int nColIn); // reshape matrix and prep for addition or subtraction of matrices of different shapes
    ECE_Matrix operator+(const ECE_Matrix &matrix_obj) const; // add two matrix objects together
    ECE_Matrix operator-(const ECE_Matrix &matrix_obj) const; // subtract one matrix object from another matrix object 
    ECE_Matrix operator+(const double &numIn) const; // add a matrix to a scalar (matrix_obj + scalar)
    friend ECE_Matrix operator+(const double &numIn, const ECE_Matrix &matrix_obj); // add a scalar to a matrix (scalar + matrix_obj)
    friend ECE_Matrix operator*(const double &numIn, const ECE_Matrix &matrix_obj); // multiply a matrix by a scalar (scalar * matrix_obj)
    ECE_Matrix transpose() const; // return transpose of matrix
    ECE_Matrix operator*(const double &numIn) const; // multiply a matrix by a scalar (matrix_obj * scalar)
    ECE_Matrix operator/(const double &numIn) const; // divide a matrix by a scalar (matrix_obj / scalar)
};

#endif