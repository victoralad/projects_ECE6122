/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Aug 23, 2019
Description:
 The Knights of the Order of Fibonacci seating order
*/

#include <iostream>
#include <vector>

bool isFib(unsigned long sumNum)
{
    //check if sumNum is a fibonacci number
    unsigned long fibN, fibN_1, fibN_2;
    fibN = 1;
    if(fibN == sumNum)
    {
        return true;
    }
    fibN_1 = 1;
    fibN_2 = 1;
    while(fibN < sumNum)
    {
        fibN = fibN_1 + fibN_2;
        if(fibN == sumNum)
        {
            return true;
        }
        fibN_2 = fibN_1;
        fibN_1 = fibN;
    }
    return false;
}
int main()
{
    unsigned long n;
    std::cout << "Please enter the number of knights: ";
    std::cin >> n;
    if (n <= 2)
    {   
        if (n == 1)
        {
            std::cout << "The knights should sit clockwise as follows: K, 1" << std::endl;
        }
        else if (n == 2)
        {
            std::cout << "The knights should sit clockwise as follows: K, 1, 2" << std::endl;
        }
    }
    else
    {
        std::vector<unsigned long> seat_arr{1, 2};
        unsigned long sumLeft, sumRight, sumFirst, sumLast;
        bool found = true;
        for (unsigned long i = 3; i <= n; i++)
        {
            sumFirst = i + seat_arr[0];
            if (isFib(sumFirst))
            {
                seat_arr.insert(seat_arr.begin(), i);
            }
            else
            {
                std::cout << "heyyyy " << i << std::endl;
                for(int j = 1; j < seat_arr.size(); j++)
                {   
                    sumLeft = i + seat_arr[j - 1];
                    sumRight = i + seat_arr[j];
                    std::cout << sumLeft << " left, right " << sumRight << std::endl;
                    if (isFib(sumLeft) && isFib(sumRight))
                    {
                        seat_arr.insert(seat_arr.begin() + j, i);
                        // std::cout << "insert" << i << std::endl;
                        break;
                    }
                    if(j == seat_arr.size() - 1)
                    {
                        seat_arr.push_back(i);
                        // std::cout << "push_back" << i << std::endl;
                        break;
                    }
                }
            }
            sumLast = i + seat_arr[seat_arr.size() - 2];
            if(!isFib(sumLast) && (i == n))
            {
                found = false;
            }
        }
        for (int i = 0; i < seat_arr.size() - 1; i++)
        {
            std::cout << seat_arr[i] << ", ";
        }
        std::cout << seat_arr[seat_arr.size() - 1] << std::endl;
        if (found)
        {
            std::cout << "The knights should sit clockwise as follows: K, ";
            for (int i = 0; i < seat_arr.size() - 1; i++)
            {
                std::cout << seat_arr[i] << ", ";
            }
            std::cout << seat_arr[seat_arr.size() - 1] << std::endl;
        }
        else
        {
            std::cout << "No solution found!" << std::endl;
        }
        
    }
    
    // 1, 1, 2, 3, 5, 8, 13, 21 fib numbers

    //  K, 4, 1, 7, 6, 2, 3, 5 correct
    // K, 4, 1, 2, 3, 5, 6, 7 my output 
  

    //4, 1, 2, 3, 5
    // 6, 7
    /*

    do
    {
        std::cout << knights[0] << knights[1] << ...
        nCount++;
        if (goodSet)
        {
            break;
        }

    }
    while
     std::next_permutation(vect.begin(), vect.end())
     std::cout << number of permutations << std::endl;
    */
    // try unordered sets 
    return 0;
}