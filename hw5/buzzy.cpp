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

// std::vector<double> shipPos(3);
// std::vector<double> shipSpeedDir(3);


void readInputData();

void readInputData(std::vector<double> &allShipInfo) 
{
    // std::vector<double> yelJacSpeed(7);
    // std::vector<std::vector<double> > yelJacPos(8, std::vector<double> (3, 0));
    // std::vector<std::vector<double> > yelJacSpeedDir(8, std::vector<double> (3, 0));

    std::ifstream input;
    input.open("in.dat");
    input >> timeOut;
    input >> maxThrust;

    // read in data for all ships 
    for (int i = 0; i < 56; ++i)
    {
       input >> allShipInfo[i];
    }
    // set all ships status to active
    for (int i = 56; i < 63; ++i) 
    {
        allShipInfo[i] = 1;
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
      
        std::vector<double> allShipInfo(63);

        readInputData(allShipInfo);

        int rc;
        for (int i = 1; i < 8; ++i)
        {
            std::cout << "Buzzy " << " sending timeOut info to Yellow Jacket " << rank + i << std::endl;
            rc = MPI_Send(&timeOut, sizeof(timeOut), MPI_INT, rank + i, 0, MPI_COMM_WORLD);
            if (rc != MPI_SUCCESS)
            {
                std::cout << "Sending timeOut info to Yellow Jacket " << rank + i << " failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }
            
            std::cout << "Buzzy " << " sending maxThrust info to Yellow Jacket " << rank + i << std::endl;
            rc = MPI_Send(&maxThrust, sizeof(maxThrust), MPI_INT, rank + i, 0, MPI_COMM_WORLD);
            if (rc != MPI_SUCCESS)
            {
                std::cout << "Sending maxThrust info to Yellow Jacket " << rank + i << " failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }

            std::cout << "Buzzy " << " sending allShipInfo to Yellow Jacket " << rank + i << std::endl;
            rc = MPI_Send(&allShipInfo, sizeof(allShipInfo), MPI_INT, rank + i, 0, MPI_COMM_WORLD);
            if (rc != MPI_SUCCESS)
            {
                std::cout << "Sending shipInfo to Yellow Jacket " << rank + i << " failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }

            std::cout << std::endl;

        }

        std::cout << "-------------------------- Initialization completed! --------------------------" << std::endl;

    }

    for (int i = 1; i < 8; ++i)
    {
        std::vector<double> allShipInfo(63);
        int rc;
        MPI_Status status;
        if (rank == i)
        {

            //   MPI_Recv(&buf, count, datatype, source, tag, comm, &status)
            rc = MPI_Recv(&timeOut, sizeof(timeOut), MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (rc != MPI_SUCCESS)
            {
                std::cout << "receive timeout info from Buzzy failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }

            rc = MPI_Recv(&maxThrust, sizeof(maxThrust), MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            if (rc != MPI_SUCCESS)
            {
                std::cout << "receive maxThrust info from Buzzy failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }

            rc = MPI_Recv(&allShipInfo, sizeof(allShipInfo), MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);      
            if (rc != MPI_SUCCESS)
            {
                std::cout << "receive allShipInfo info from Buzzy failed, rc " << rc <<std::endl;
                MPI_Finalize();
                exit(1);
            }

            for (int i = 0; i < allShipInfo.size(); ++i) 
            {
                std::cout << allShipInfo[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    while (timeOut > 0)
    {
        timeOut--;
    }

    MPI_Finalize();

    
    return 0;
}