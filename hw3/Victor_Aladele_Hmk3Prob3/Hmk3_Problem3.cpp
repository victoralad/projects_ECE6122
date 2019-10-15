/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 1, 2019
Description:
 finding the maximum path sum, starting at the top of a given triangle
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>

void maxPathSum(std::vector<std::vector<int> > &triangle); // declaration of function to compute max path sum

//  Function for finding maximum sum using a dynamic programming approach
//  Function computes max sum bottom-up, with the max sum = triangle[0][0]
void maxPathSum(std::vector<std::vector<int> > &triangle) 
{ 
    int i, j;
    // loop for moving from bottom to the top of the triangle
    for (i = triangle.size() - 1; i > 0; i--) 
    { 
        omp_set_num_threads(1);
#pragma omp parallel for private(j)  // omp parallelization
        // loop for moving across row i
        for (j = 0; j <= i; j++) 
        { 
            // for each element (x), check both elements right below the number and below the number to the right (a, b)
            // add the maximum of the two numbers max(a, b) to the number x
            triangle[i - 1][j] += std::max(triangle[i][j], triangle[i][j + 1]); 
        } 
    } 
    // 164437
} 

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Please pass in one argument for the name of the input file" << std::endl;
        return 0;
    }

    int triang_height = 0, nCols = 0;
    std::ifstream data_triangle;
    data_triangle.open(argv[1]);
    data_triangle >> triang_height;

    // create 2D vector to hold all the values from from the input data file. Initialize all vector entries to zero
    std::vector <std::vector<int> > triangle(triang_height, std::vector<int>(triang_height, 0));
    for (int i = 0; i < triang_height; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            data_triangle >> triangle[i][j]; // read in triangle data from inputed text file and save the values to 2D vector 'triangle'
        }
    }
    data_triangle.close();
    double start_time = omp_get_wtime();
    maxPathSum(triangle); // call function to compute maximum path sum
    std::cout << 1000.0*(omp_get_wtime() - start_time) << " millsec with main thread" << std::endl;

    std::ofstream data_max_pathSum;
    data_max_pathSum.open("output3.txt");
    data_max_pathSum << triangle[0][0]; // write maximum path sum to file named output3.txt
    data_max_pathSum.close();
    return 0;
}