/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 27, 2019
Description:
 A program that determines the number of possible paths through 
 a given lattice of height H and width W, as quickly as possible.
*/

#include <iostream>
#include <vector>
#include <thread>
#include <sstream>

unsigned long long num_path = 0; // final answer
void numOfPaths(unsigned long long height, unsigned long long width);

void numOfPaths(unsigned long long height, unsigned long long width)
{
    // 2D vector to store the number of paths as the calculation is propagated from top left to bottom right of lattice
    std::vector<std::vector<unsigned long long> > counter(height + 1, std::vector<unsigned long long> (width + 1, 0));
    
  
    // The number of paths to reach any vertex in the first column is 1
    for (unsigned long long i = 0; i < height + 1; i++) 
    {
        counter[i][0] = 1; 
    }
    
    // The number of paths to reach any vertex in the first row is 1
    for (unsigned long long j = 0; j < width + 1; j++) 
    {
        counter[0][j] = 1; 
    }
  
    // The number of paths for other cells propagated
    // from top left corner to bottom right corner
    for (unsigned long long i = 1; i < height + 1; i++) { 
        for (unsigned long long j = 1; j < width + 1; j++) 
        {
            counter[i][j] = counter[i - 1][j] + counter[i][j - 1];
        }
    } 

    num_path = counter[height][width]; // final answer
}

int main(int argc, char **argv)
{
    unsigned long long height, width;

    if (argc != 5)
    {
        std::cout << "Please enter the command line arguments with flags: -h for height and -w for width" << std::endl;
        return 0;
    }

    // used to convert char* to unsigned long long
    std::stringstream ss2(argv[2]); 
    std::stringstream ss4(argv[4]); 

    if (argv[1][1] == 'h') 
    {
        ss2 >> height;
        ss4 >> width;
    }
    else
    {
        ss4 >> height;
        ss2 >> width;
    }

    std::thread t1(numOfPaths, height, width);  // initialize thread with entry entry point: 'numOfPaths
    t1.join();  // ensures thread t1 is finished before main thread continues 

    std::cout << "Number of Routes: " << num_path << std::endl;

    return 0;
}

// Google how to see the number of running threads in terminal
