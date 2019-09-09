/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Aug 23, 2019
Description:
 The Knights of the Order of Fibonacci seating order
*/

#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <vector>

std::unordered_set<unsigned long> fib_set; // create container (unordered_set) to store fibonacci numbers for easy lookup
bool possible = false; // global boolean variable to check if a solution is found.

bool isFib(unsigned long sumNum);
std::vector<unsigned long> seat_arr(std::vector<unsigned long> &vec);

std::vector<unsigned long> seat_arr(std::vector<unsigned long> &vec)
{
    bool perm_isFib = true;
    do
    {
        for(int i = 1; i < vec.size(); i++)
        {
            perm_isFib = isFib(vec[i - 1] + vec[i]);
            if (!perm_isFib)
            {
                break;
            }
        }
        if (perm_isFib)
        {
            possible = true;
            return vec;
        }
    } while (std::next_permutation(vec.begin(), vec.end()));
    
    return vec;
}

bool isFib(unsigned long sumNum)
{
    //check if sumNum is a fibonacci number
    if(fib_set.find(sumNum) == fib_set.end())
    {
        return false;
    }
    else
    {
        return true;
    }
}
int main()
{
    unsigned long n;
    std::cout << "Please enter the number of knights: ";
    std::cin >> n;

    // create a list of possible fibonacci numbers for a given sequence 1, ..., n
    unsigned long fibN, fibN_1, fibN_2;
    fibN = 1;
    fibN_1 = 1;
    fibN_2 = 1;
    do
    {
        fib_set.insert(fibN);
        fibN = fibN_1 + fibN_2;
        fibN_2 = fibN_1;
        fibN_1 = fibN;
    } while (fibN < (2*n)); // ensures that only fibonacci numbers that are possible with the given sequence are added to the container
    
    std::vector<unsigned long> vec(n);
    for (int i = 0; i < n; i++)
    {
        vec[i] = i + 1;
    }
    
    vec = seat_arr(vec);
    // check if a solution is possible
    if (possible)
    {
        std::cout << "The knights should sit clockwise as follows: K, ";
        if(vec[0] > vec[vec.size() - 1])
        {
            std::reverse(vec.begin(), vec.end());
        }
        for(int i = 0; i < n; i++)
        {
            std::cout << vec[i] << " ";
        }
        std::cout << "\n";
    }
    else
    {
        std::cout << "“No solution found!" << std::endl;
    }
    
    return 0;
}