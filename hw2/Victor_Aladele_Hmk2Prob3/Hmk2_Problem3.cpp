/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 6, 2019
Description:
 finding the maximum path sum, starting at the top of a given triangle
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

    int triang_height = 0, nCols = 0;
    std::ifstream data_triangle;
    data_triangle.open(argv[1]);
    data_triangle >> triang_height;

    std::vector<std::vector<int>> triangle(triang_height);
    for (int i = 0; i < triang_height; i++)
    {
        triangle[i].resize(i + 1);
        for (int j = 0; j < i + 1; j++)
        {
            data_triangle >> triangle[i][j];
        }
    }
    data_triangle.close();

    for (int i = 0; i < triang_height; i++)
    {
        for (int j = 0; j < triangle[i].size(); j++)
        {
            std::cout << triangle[i][j] << " ";
        }
        std::cout << "\n";
    }

    int max_pathSum = 0;
    std::ofstream data_max_pathSum;
    data_max_pathSum.open("output3.txt");
    data_max_pathSum << max_pathSum;
    data_max_pathSum.close();


    return 0;
}