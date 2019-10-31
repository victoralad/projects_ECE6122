// Demonstrate simple MPI program
// This one uses non-blocking ISend/Irecv
// George F. Riley, Georgia Tech, Fall 2011


#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>


#define  MASTER  0
#define SHIPMASS 10000;

int timeOut; 
double maxThrust;
double shipSpeed;
int shipStatus = 1; // set all ships status to active
int recvAllShipStatus[8] = {0};
double allShipInfo[56] = {0};
double recvAllShipInfo[56] = {0};
double shipInfo[7];

void CalculateBuzzyXYZ();

void CalculateYellowJacketXYZ();

void readInputData();

void CalculateBuzzyXYZ()
{
    // double buzzyShipInfo[8];
    // calculate new position of Buzzy using Newton's equation x1 = x0 + v*t
    allShipInfo[2] += allShipInfo[3]; 
}

void CalculateYellowJacketXYZ()
{
    std::cout << "hey" << std::endl;
}

void readInputData() 
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

    input.close();
}

int main(int argc, char**argv)
{
    int  numtasks, rank, rc;
    const int root = 0;

    rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Seed the random number generator to get different results each time
    srand(rank);
    if (rank == 0)
    {
        // Load in.dat file
        readInputData();
        
    }
    
    timeOut = 1;
    // Broadcast to yellowjackets
    MPI_Bcast(&timeOut, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&maxThrust, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Bcast(allShipInfo, 56, MPI_DOUBLE, root, MPI_COMM_WORLD);

    // Loop through the number of time steps
    for (int round = 0; round < timeOut; ++round)
    {
        // std::cout << "round " << round << std::endl;

        if (rank == 0)
        {
            // Calculate Buzzy new location
            // CalculateBuzzyXYZ();
            // std::cout << round << std::endl;
            MPI_Allgather(&shipStatus, 1, MPI_INT, recvAllShipStatus, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Allgather(shipInfo, 7, MPI_DOUBLE, recvAllShipInfo, 7, MPI_DOUBLE, MPI_COMM_WORLD);
        }
        else
        {
            // Calculate yellow jacket new location
            // CalculateYellowJacketXYZ();
            MPI_Allgather(&shipStatus, 1, MPI_INT, recvAllShipStatus, 1, MPI_INT, MPI_COMM_WORLD);
            // shipStatus = recvAllShipStatus[rank];
            
            MPI_Allgather(shipInfo, 7, MPI_DOUBLE, recvAllShipInfo, 7, MPI_DOUBLE, MPI_COMM_WORLD);
            // std::cout << shipStatus << " " << std::endl;
            for (int i = 7*rank; i < 7*rank + 7; ++i)
            {
                std::cout << recvAllShipInfo[i] << " ";
            }
            std::cout << std::endl;
        }
    }

    MPI_Finalize();
}
