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
#define SHIPMASS 10000;

int timeOut; 

double maxThrust;
double shipSpeed;
std::vector<double> shipPos(3);
std::vector<double> shipSpeedDir(3);

void readInputData();

void readInputData() 
{
    std::vector<double> yelJacSpeed(7);
    std::vector<std::vector<double> > yelJacPos(7, std::vector<double> (3, 0));
    std::vector<std::vector<double> > yelJacSpeedDir(7, std::vector<double> (3, 0));

    std::ifstream input;
    input.open("in.dat");
    input >> timeOut;
    input >> maxThrust;

    // read in data for just Buzzy
    for (int k = 0; k < 3; ++k) 
    {
        input >> shipPos[k];
    }

    input >> shipSpeed;

    for (int k = 0; k < 3; ++k) 
    {
        input >> shipSpeedDir[k];
    }

    // read in data for the 7 Yellow Jackets 
    for (int i = 0; i < 7; ++i)
    {
        for (int j = 0; j < 3; ++j) 
        {
            input >> yelJacPos[i][j];
        }

        input >> yelJacSpeed[i];

        for (int j = 0; j < 3; ++j) 
        {
            input >> yelJacSpeedDir[i][j];
        }
    }
    input.close();
}

int main(int argc, char *argv[])
{

    
    int numTasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];

    
    MPI_Init(&argc, &argv);
    

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(hostname, &len);

    
    if (rank == MASTER)
    {
        readInputData();
        std::cout << "maxThrust" << rank << ": " << maxThrust << std::endl;
        // for (int i = 0; i < 7; ++i)
        // {
        //     cout << "Rank " << rank << " sending to rank " << rank + 1 << endl;
        //     rc = MPI_Send(  buf,            // void* data
        //                     sizeof(buf),    // int count
        //                     MPI_CHAR,       // MPI_Datatype datatype
        //                     rank + 1,       // int destination
        //                     0,              // int Message tag ID
        //                     MPI_COMM_WORLD);// MPI_Comm communicator
        //     if (rc != MPI_SUCCESS)
        //     {
        //         cout << "Rank " << rank
        //             << " send failed, rc " << rc << endl;
        //         MPI_Finalize();
        //         exit(1);
        //     }
        // }

    }

    for (int i = 1; i < 8; ++i)
    {
        if (rank == i)
        {
            // std::cout << "maxThrust" << rank << ": " << maxThrust << std::endl;
        }
    }

    while (timeOut > 0)
    {
        timeOut--;
    }

    MPI_Finalize();

    
    return 0;
}