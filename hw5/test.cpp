// Demonstrate simple MPI program
// This one uses non-blocking ISend/Irecv
// George F. Riley, Georgia Tech, Fall 2011


#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>


#define SHIPMASS 10000;

int numtasks, rank, rc;
const int root = 0;

int timeOut; 
double maxThrust;
double allShipInfo[56] = {0};

const int posLen = 3;
const int velLen = 4;
const int forceLen = 3;

int shipStatus = 1;                 // set all ships status to active
double shipPos[posLen] = {0};           
double shipVel[velLen] = {0};
double shipForce[forceLen] = {0};

int recvAllShipStatus[8] = {0};
double recvAllShipPos[24] = {0};
double recvAllShipVel[32] = {0};   
double recvAllShipForce[24] = {0};

void CalculateBuzzyXYZ();

void CalculateYellowJacketXYZ();

void readInputData();

void CalculateBuzzyXYZ()
{
    // calculate new position of Buzzy using Newton's equation s1 = s0 + v*t
    shipPos[0] += shipVel[0] * shipVel[1]; // x
    shipPos[1] += shipVel[0] * shipVel[2]; // y
    shipPos[2] += shipVel[0] * shipVel[3]; // z
}

void CalculateYellowJacketXYZ()
{
    
}

void readInputData() 
{
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

    rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS)
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Seed the random number generator to get different results each time
    // srand(rank);
    if (rank == 0)
    {
        // Load in.dat file
        readInputData();
        
    }
    
    timeOut = 2;
    // Broadcast to yellowjackets
    MPI_Bcast(&timeOut, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&maxThrust, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Bcast(allShipInfo, 56, MPI_DOUBLE, root, MPI_COMM_WORLD);

    // set the initial ship velocity for all ships from allShipInfo
    for (int i = 0; i < 3; ++i)
    {
        shipPos[i] = allShipInfo[7*rank + i];
    }
    for (int i = 3; i < 5; ++i)
    {
        shipVel[i] = allShipInfo[7*rank + i]; 
    }
    
    // Loop through the number of time steps
    for (int round = 0; round < timeOut; ++round)
    {
        if (rank == 0)
        {
            // Calculate Buzzy new location
            CalculateBuzzyXYZ();

            // Share information with other yellow jackets
            MPI_Allgather(&shipStatus, 1, MPI_INT, &recvAllShipStatus, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Allgather(shipPos, 3, MPI_DOUBLE, recvAllShipPos, 3, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Allgather(shipVel, 4, MPI_DOUBLE, recvAllShipVel, 4, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Allgather(shipForce, 3, MPI_DOUBLE, recvAllShipForce, 3, MPI_DOUBLE, MPI_COMM_WORLD);

            // Output all ship info to console:  rankID, status, x, y, z, F x , F y , F z
            std::cout << "----------------------- Round " << round << " -------rankID, status, x, y, z, Fx , Fy , Fz -----------------------" << std::endl;
            for (int i = 1; i < numtasks; ++i)
            {
                std::cout << i << " " << recvAllShipStatus[i] << " ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipPos[8*i + j] << " ";
                }
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipForce[8*i + j] << " ";
                }
                std::cout << std::endl;
            }
        }
        else
        {
            // Calculate yellow jacket new location
            // CalculateYellowJacketXYZ();
            MPI_Allgather(&shipStatus, 1, MPI_INT, &recvAllShipStatus, 1, MPI_INT, MPI_COMM_WORLD);
            MPI_Allgather(shipPos, 3, MPI_DOUBLE, recvAllShipPos, 3, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Allgather(shipVel, 4, MPI_DOUBLE, recvAllShipVel, 4, MPI_DOUBLE, MPI_COMM_WORLD);
            MPI_Allgather(shipForce, 3, MPI_DOUBLE, recvAllShipForce, 3, MPI_DOUBLE, MPI_COMM_WORLD);
            
        }
    }

    MPI_Finalize();
}
