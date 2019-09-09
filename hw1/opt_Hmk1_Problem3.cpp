/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Sep 1, 2019
Description:
 The Knights of the Order of Fibonacci seating order
*/

#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <vector>

std::unordered_set<unsigned long> fib_set; // create container (unordered_set) to store fibonacci numbers for easy lookup
std::vector<unsigned long> valid_seat;  // vector that holds the valid seating arrangement
bool possible = false;   // global boolean variable to check if a solution is found. 

bool isFib(unsigned long sumNum);   // check if a number is a fibonacci number i.e. checks if a number is contained in the unordered_set container
void seat_arr(std::vector<unsigned long> &vec); // helper function to find a valid seating arrangement
bool isGoodSet(std::vector<unsigned long> &vec_isFib); // check if a vector satisfies the fibonancci condition
bool recurse(std::vector<unsigned long> &vec); // a permutation function via recursion to guide the seatch for a valid seating arrangement

// https://www.geeksforgeeks.org/permutations-of-a-given-string-using-stl/
bool recurse(std::vector<unsigned long> &vec, unsigned long tied)
{
    unsigned long subVecSize = vec.size() - tied;
    std::vector<unsigned long> vec_isFib(vec.end() - tied, vec.end());

    if(subVecSize == 0) // check if the left side of the vector has been used up. i.e [1, 2, 3, 4 |] -> [1, 2, 3 | 4] -> [1, 2 | 3, 4] -> ... [| 1, 2, 3, 4]
    {
        for(unsigned long i = 0; i < vec.size(); i++)
        {
            std::cout << vec[i] << " ";
        }
        std::cout << "\n";
        return possible;
    }

    else if (isGoodSet(vec) == true) // check if vector satisifies the Fibonacci condition
    {
        for(unsigned long i = 0; i < vec.size(); i++)
        {
            valid_seat[i] = vec[i];
        }
        possible = true;
        return possible;
    }

    else if (tied > 1 && isGoodSet(vec_isFib) == false) // check if right side of the vector satisfies the fibonacci condition. If it fails, prune that part of the tree
    {
        return possible;
    }

    for(unsigned long i = 0; i < subVecSize; i++)
    {
        recurse(vec, tied + 1);
        std::rotate(vec.begin(), vec.begin() + 1, vec.end() - tied);
    }
    
}

bool isFib(unsigned long sumNum)
{
    //check if sumNum is contained in the set container
    if(fib_set.find(sumNum) == fib_set.end())
    {
        return false;
    }
    
    else
    {
        return true;
    }
}

bool isGoodSet(std::vector<unsigned long> &vec_isFib)
{
    bool perm_isFib = true; 
    // checks to see if the sum of each ajacent numbers in sequence is a Fibonacci number 
    for(unsigned long i = 1; i < vec_isFib.size(); i++)
    {
        perm_isFib = isFib(vec_isFib[i - 1] + vec_isFib[i]);
        if (perm_isFib == false)
        {
            break;
        }
    }
    return perm_isFib;
}

void seat_arr(std::vector<unsigned long> &vec, unsigned long index)
{
    // first checks to see if sequence is a valid sequence
    if (isGoodSet(vec))
    {
        for(unsigned long i = 0; i < vec.size(); i++)
        {
            valid_seat[i] = vec[i];
        }
        possible = true;
    }
    
    else
    {
        for(unsigned long i = 0; i < vec.size(); i++)
        {
            std::rotate(vec.begin(), vec.begin() + 1, vec.end()); // [1, 2, 3, 4] becomes [2, 3, 4, 1]
            recurse(vec, 1);
        }
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
        fib_set.insert(fibN); // add fibonacci number to container
        fibN = fibN_1 + fibN_2;
        fibN_2 = fibN_1;
        fibN_1 = fibN;
    } while (fibN < (2*n));  // ensures that only fibonacci numbers that are possible with the given sequence are added to the container
    
    //create sequence 1, ..., n
    std::vector<unsigned long> vec(n);
    for (unsigned long i = 0; i < n; i++)
    {
        vec[i] = i + 1;
    }
    
    valid_seat.resize(n);
    seat_arr(vec, 0);

    // check if a solution is possible
    if (possible)
    {
        std::cout << "The knights should sit clockwise as follows: K, ";

        if(valid_seat[0] > valid_seat[valid_seat.size() - 1])  // check to see if the result is in the right order (clockwise or counter-clockwise)
        {
            std::reverse(valid_seat.begin(), valid_seat.end()); // if result is counter-clockwise, reverse the order
        }

        for(unsigned long i = 0; i < n; i++)   // print output
        {
            std::cout << valid_seat[i] << " ";
        }
        std::cout << "\n";
    }

    else
    {
        std::cout << "“No solution found!" << std::endl;
    }
    
    return 0;
}