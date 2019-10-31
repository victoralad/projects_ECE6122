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
int shipStatus = 1;                 // set all ships status to active
double allShipInfo[56] = {0};
double shipVel[4] = {0};
double recvAllshipVel[32] = {0};
double force[3] = {0};
double shipInfo[8] = {0};           // rankID, status, x, y, z, Fx , Fy , Fz
double recvAllShipInfo[64] = {0};   // rankID, status, x, y, z, Fx , Fy , Fz  * 8

void CalculateBuzzyXYZ();

void CalculateYellowJacketXYZ();

void readInputData();

void CalculateBuzzyXYZ()
{
    // calculate new position of Buzzy using Newton's equation s1 = s0 + v*t
    shipInfo[2] += shipVel[0] * shipVel[1]; // y
    shipInfo[3] += shipVel[0] * shipVel[2]; // z
    shipInfo[4] += shipVel[0] * shipVel[3]; // z
}

void CalculateYellowJacketXYZ()
{
    shipInfo[1] = shipStatus;
    for (int i = 2; i < 5; ++i)
    {
        shipInfo[i] = allShipInfo[7*rank + i];
    }
    for (int i = 5; i < 8; ++i)
    {
        shipInfo[i] = force[i - 5];
    }
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
    for (int i = 0; i < 4; ++i)
    {
        shipVel[i] = allShipInfo[i + 3];
    }

    // set the initial values to be printed to the screen by Buzzy
    shipInfo[0] = rank;
    shipInfo[1] = shipStatus;
    for (int i = 2; i < 5; ++i)
    {
        shipInfo[i] = allShipInfo[7*rank + i - 2]; 
    }
    for (int i = 5; i < 8; ++i)
    {
        shipInfo[i] = force[i - 5];
    }

    // Loop through the number of time steps
    for (int round = 0; round < timeOut; ++round)
    {
        if (rank == 0)
        {
            MPI_Allgather(shipVel, 4, MPI_DOUBLE, recvAllshipVel, 4, MPI_DOUBLE, MPI_COMM_WORLD);

            // Calculate Buzzy new location
            CalculateBuzzyXYZ();

            // Share information with other yellow jackets
            MPI_Allgather(shipInfo, 8, MPI_DOUBLE, recvAllShipInfo, 8, MPI_DOUBLE, MPI_COMM_WORLD);

            // Output all ship info to console:  rankID, status, x, y, z, F x , F y , F z
            std::cout << "----------------------- Round " << round << " -------rankID, status, x, y, z, Fx , Fy , Fz -----------------------" << std::endl;
            for (int i = 0; i < numtasks; ++i)
            {
                for (int j = 0; j < 8; ++j)
                {
                    std::cout << recvAllShipInfo[8*i + j] << " ";
                }
                std::cout << std::endl;
            }
        }
        else
        {
            MPI_Allgather(shipVel, 4, MPI_DOUBLE, recvAllshipVel, 4, MPI_DOUBLE, MPI_COMM_WORLD);
            // Calculate yellow jacket new location
            // CalculateYellowJacketXYZ();

           
            MPI_Allgather(shipInfo, 8, MPI_DOUBLE, recvAllShipInfo, 8, MPI_DOUBLE, MPI_COMM_WORLD);
            
        }
    }

    MPI_Finalize();
}
