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

    std::vector<std::vector<int>> matrix(nRows);
    for (int i = 0; i < nRows; i++)
    {
        matrix[i].resize(nCols);
        for (int j = 0; j < nCols; j++)
        {
            data_matrix >> matrix[i][j];
        }
    }
    data_matrix.close();

    // for (int i = 0; i < nRows; i++)
    // {
    //     for (int j = 0; j < nCols; j++)
    //     {
    //         std::cout << matrix[i][j] << " ";
    //     }
    //     std::cout << "\n";
    // }

    int max_product = 0;
    std::ofstream data_max_product;
    data_max_product.open("output2.txt");
    data_max_product << max_product;
    data_max_product.close();


    return 0;
}