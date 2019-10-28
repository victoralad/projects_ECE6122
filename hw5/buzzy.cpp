/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 28, 2019
Description:
    A distributed MPI program to simulate the docking of 
    spacehips (Yellow Jackets) with the mothership (Buzzy)
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

#define  MASTER  0

double shipSpeed;
std::vector<double> shipPos(3);
std::vector<double> shipSpeedDir(3);

void readInputData();

void readInputData() 
{
    int timeOut; 
    double maxThrust;
    double allShipSpeed;
    std::vector<double> allShipPos(3);
    std::vector<double> allShipSpeedDir(3);

    std::ifstream input;
    input.open("in.dat");
    input >> timeOut;
    input >> maxThrust;
    for (int i = 0; i < 3; ++i) 
    {
        input >> allShipPos[i]
    }
    input >> allShipSpeed;
    for (int i = 0; i < 3; ++i) 
    {
        input >> allShipSpeedDir[i]
    }
    input.close();
}

int main(int argc, char *argv[])
{

    
    int numTasks, taskId, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    
    MPI_Init(&argc, &argv);
    

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskId);
    MPI_Get_processor_name(hostname, &len);

    
    if (taskId == MASTER)
    {
        printf("MASTER: Number of MPI tasks is: %d\n", numTasks);
        readInputData();

    }

    while (timeOut > 0)
    {

        timeOut--;
    }

    MPI_Finalize();

    
    return 0;
}