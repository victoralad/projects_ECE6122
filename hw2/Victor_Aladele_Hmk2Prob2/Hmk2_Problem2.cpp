/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 6, 2019
Description:
 determine the largest product of four adjacent numbers in the same
 direction (up, down, left, right, or diagonally)
*/

#include <iostream>
#include <fstream>
#include <vector>

long long maxAdjProduct(std::vector <std::vector<long long> > &matrix); //function to compute max adjacent product

// function to find max adjacent product 
long long maxAdjProduct(std::vector <std::vector<long long> > &matrix) 
{ 
    long long result;
    long long maxProduct = 0; 
    long long nRows = matrix.size();
    long long nCols = matrix[0].size();
  
    // loop through the rows. 
    for (long long i = 0; i < nRows; i++)  
    { 
        // loop through the columns. 
        for (long long j = 0; j < nCols; j++)  
        { 
            // Check the maximum product in i-th row from right to left. 
            // ensure that there are at least four numbers in that direction
            if (j >= 3)  
            { 
                result = matrix[i][j] * matrix[i][j - 1] * matrix[i][j - 2] * matrix[i][j - 3]; 
                if (maxProduct < result)
                {
                    maxProduct = result; 
                }
            } 
            // Check the maximum product in j-th column from bottom to top. 
            // ensure that there are at least four numbers in that direction 
            if (i >= 3)  
            { 
                result = matrix[i][j] * matrix[i - 1][j] * matrix[i - 2][j] * matrix[i - 3][j]; 
                if (maxProduct < result) 
                {
                    maxProduct = result; 
                }
            } 
            // check the maximum product in the diagonal from bottom-right to top-left and
            // check the maximum product in the anti-diagonal from bottom-left to top-right
            if (i >= 3 && j >= 3)  
            { 
                // check diagonal
                result = matrix[i][j] * matrix[i - 1][j - 1] *  matrix[i - 2][j - 2] * matrix[i - 3][j - 3]; 
                if (maxProduct < result)
                {
                    maxProduct = result; 
                } 

                // check anti-diagonal
                result = matrix[i][j] * matrix[i - 1][j + 1] *  matrix[i - 2][j + 2] * matrix[i - 3][j + 3]; 
                if (maxProduct < result)
                {
                    maxProduct = result; 
                } 
            } 
        } 
    } 
    return maxProduct; 
} 

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Please pass in one argument for the name of the input file" << std::endl;
        return 0;
    }

    long long nRows = 0, nCols = 0;
    std::ifstream data_matrix;
    data_matrix.open(argv[1]);
    data_matrix >> nRows >> nCols;

    // create 2D vector to hold all the values from from the input data file. Initialize all vector entries to zero
    std::vector <std::vector<long long> > matrix(nRows, std::vector<long long>(nCols, 0));
    for (long long i = 0; i < nRows; i++)
    {
        for (long long j = 0; j < nCols; j++)
        {
            data_matrix >> matrix[i][j];
        }
    }
    data_matrix.close();

    long long maxProduct = maxAdjProduct(matrix); // call function to compute maximum adjacent product

    std::ofstream data_max_product;
    data_max_product.open("output2.txt");
    data_max_product << maxProduct; // write maximum adjacent product to file named output3.txt
    data_max_product.close();

    return 0;
}