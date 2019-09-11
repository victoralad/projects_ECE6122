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

int maxAdjProduct(std::vector <std::vector<int>> &matrix);

// function to find maxProductproduct 
int maxAdjProduct(std::vector <std::vector<int>> &matrix) 
{ 
    int result;
    int maxProduct = 0; 
    int nRows = matrix.size();
    int nCols = matrix[0].size();
  
    // iterate the rows. 
    for (int i = 0; i < nRows; i++)  
    { 
        // iterate the columns. 
        for (int j = 0; j < nCols; j++)  
        { 
            // check the maximum product  
            // in horizontal row. 
            if ((j - 3) >= 0)  
            { 
                result = matrix[i][j] * matrix[i][j - 1] * 
                    matrix[i][j - 2] * matrix[i][j - 3]; 
                  
                if (maxProduct< result)
                    maxProduct= result; 
            } 
  
            // check the maximum product  
            // in vertical row. 
            if ((i - 3) >= 0)  
            { 
                result = matrix[i][j] * matrix[i - 1][j] * 
                    matrix[i - 2][j] * matrix[i - 3][j]; 
                  
                if (maxProduct< result) 
                    maxProduct= result; 
            } 
  
            // check the maximum product in 
            // diagonal and anti - diagonal 
            if ((i - 3) >= 0 && (j - 3) >= 0)  
            { 
                result = matrix[i][j] * matrix[i - 1][j - 1] * 
                    matrix[i - 2][j - 2] * matrix[i - 3][j - 3]; 
                  
                if (maxProduct< result) 
                    maxProduct= result; 
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

    int nRows = 0, nCols = 0;
    std::ifstream data_matrix;
    data_matrix.open(argv[1]);
    data_matrix >> nRows >> nCols;

    std::vector <std::vector<int>> matrix(nRows, std::vector<int>(nCols, 0));
    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            data_matrix >> matrix[i][j];
        }
    }
    data_matrix.close();

    for (int i = 0; i < nRows; i++)
    {
        for (int j = 0; j < nCols; j++)
        {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << "\n";
    }

    int maxProduct = maxAdjProduct(matrix);
    std::ofstream data_max_product;
    data_max_product.open("output2.txt");
    data_max_product << maxProduct;
    data_max_product.close();


    return 0;
}