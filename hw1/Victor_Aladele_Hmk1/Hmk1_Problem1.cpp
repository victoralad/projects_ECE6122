/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Aug 22, 2019
Description:
Takes in a number numIn from the console and outputs to the
console the number of 1's that must be added to the positive integer numIn before a particular process ends. 
The process consists of dividing by 7 if number is divisible by 7, or adding one if not.
*/

#include <iostream>

// Recursive function that returns the number of times 1's were added to the input numIn
int countAddOne(int numIn, int count)
{
    if (numIn == 1){
        return count;
    }
    else if((numIn % 7) == 0) // if numIn is divisible by 7
    {
        numIn = numIn/7;
        countAddOne(numIn, count);
    }
    else
    {
        numIn++;
        count++;
        countAddOne(numIn, count);
    }
    
}
int main()
{
    int numIn;
    int count = 0;
    std::cout << "Please enter the starting number numIn: ";
    std::cin >> numIn;  // accepts user input
    count = countAddOne(numIn, count);
    std::cout << "The sequence had "<< count << " instances of the number 1 being added" << std::endl;
    return 0;
}