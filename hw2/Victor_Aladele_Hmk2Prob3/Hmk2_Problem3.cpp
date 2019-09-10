/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 10, 2019
Description:
 finding the maximum path sum, starting at the top of a given triangle
*/

#include <iostream>
#include <fstream>
#include <vector>

void maxPathSum(std::vector<std::vector<int>> &triangle); // declaration of function to compute max path sum

//  Function for finding maximum sum using a dynamic programming approach
//  Function computes max sum bottom-up, with the max sum = triangle[0][0]
void maxPathSum(std::vector<std::vector<int>> &triangle) 
{ 
     // loop for moving from bottom to the top of the triangle
     for (int i = triangle.size() - 1; i > 0; i--) 
     { 
        // loop for moving across row i
        for (int j = 0; j <= i; j++) 
        { 
            // for each element (x), check both elements right below the number and below the number to the right (a, b)
            // add the maximum of the two numbers max(a, b) to the number x
            triangle[i - 1][j] += std::max(triangle[i][j], triangle[i][j + 1]); 
        } 
     } 
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

    std::vector <std::vector<int> > triangle(triang_height, std::vector<int>(triang_height, 0));
    for (int i = 0; i < triang_height; i++)
    {
        for (int j = 0; j < i + 1; j++)
        {
            data_triangle >> triangle[i][j]; // read in triangle data from inputed text file and save the values to 2D vector 'triangle'
        }
    }
    data_triangle.close();

    maxPathSum(triangle); // call function to compute maximum path sum
    
    std::ofstream data_max_pathSum;
    data_max_pathSum.open("output3.txt");
    data_max_pathSum << triangle[0][0]; // write maximum path sum to file named output3.txt
    data_max_pathSum.close();
    return 0;
}