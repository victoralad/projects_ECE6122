/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Aug 23, 2019
Description:
Takes in a number numIn and step-size 'numSubDiv' from the console and computes the numerical
integration of the given function
*/

#include <iostream>

// function evaluating the integral of 4/(1+xx) and returning the total integral
double calculate_integral(double beta, unsigned long numSubDiv)
{
    double step_size = beta/numSubDiv;  // width of each rectangle
    double x_i = 0.5 * step_size;  // mid-point of first rectangle
    double integral = (4 * step_size) / (1 + x_i * x_i);;
    for(int i = 1; i < numSubDiv; i++) // total sum of area of all rectangles covered by the function
    {
        x_i = x_i + step_size;
        integral += (4 * step_size) / (1 + x_i * x_i); // area of each rectangle
    }
    return integral;
}

int main()
{
    double beta;  // upper limit of integral
    unsigned long numSubDiv;  // number of subdivisions 
    double integral;

    std::cout << "Please enter a value for the upper limit (beta): ";
    std::cin >> beta;
    std::cout << "Please enter the number of subdivisions to use: ";
    std::cin >> numSubDiv;

    integral = calculate_integral(beta, numSubDiv);
    std::cout << "The integral evaluates to: " << integral << std::endl;
    
    return 0;
}