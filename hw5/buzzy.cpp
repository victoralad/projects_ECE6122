/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Oct 28, 2019
Description:
    A distributed MPI program to simulate the docking of 
    spacehips (Yellow Jackets) with the mothership (Buzzy)
    The status of all ships is continually updated and monitored
    Active = 1, Docked = 2, Destroyed = 0
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
    std::vector<std::vector<double> > allShipInfo(8, std::vector<double> (8, 0));
    // std::vector<std::vector<double> > yelJacPos(8, std::vector<double> (3, 0));
    // std::vector<std::vector<double> > yelJacSpeedDir(8, std::vector<double> (3, 0));

    std::ifstream input;
    input.open("in.dat");
    input >> timeOut;
    input >> maxThrust;

    // read in data for all ships 
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 7; ++j) 
        {
            input >> allShipInfo[i][j];
            // std::cout << allShipInfo[i][j] << " ";
        }
        // set all ships status to active
        allShipInfo[i][8] = 1;
        // std::cout << allShipInfo[i][8] << std::endl;
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
        for (int i = 0; i < 7; ++i)
        {
            cout << "Rank " << rank << " sending to rank " << rank + 1 << endl;
            rc = MPI_Send(  buf,            // void* data
                            sizeof(buf),    // int count
                            MPI_CHAR,       // MPI_Datatype datatype
                            rank + 1,       // int destination
                            0,              // int Message tag ID
                            MPI_COMM_WORLD);// MPI_Comm communicator
            if (rc != MPI_SUCCESS)
            {
                cout << "Rank " << rank
                    << " send failed, rc " << rc << endl;
                MPI_Finalize();
                exit(1);
            }
        }

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