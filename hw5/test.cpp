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
const int velLen = 3;
const int forceLen = 3;

int shipStatus = 1;                 // set all ships status to active
double shipPos[posLen] = {0};           
double shipVel[velLen] = {0};
double shipForce[forceLen] = {0};

int recvAllShipStatus[8] = {0};
double recvAllShipPos[24] = {0};
double recvAllShipVel[24] = {0};   
double recvAllShipForce[24] = {0};

bool shipDestroyed = false;
bool allShipsDocked = true;

double mKp = 5, mKd = 2.5; // PD controller gains
double errPos[posLen] = {0};
double errVel[velLen] = {0};
double errInt[3] = {0}; // Integral error that is proportional to the velocity of Buzzy
double shipAccel[3] = {0};

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
    // use PD controller  // note time = 1 unit
    for (int i = 0; i < 3; ++i)
    {
        errPos[i] = recvAllShipPos[i] - shipPos[i];
        errVel[i] = recvAllShipVel[i] - shipVel[i];
        errInt[i] = recvAllShipVel[i];
        shipForce[i] = mKp * errPos[i]  +  mKd * errVel[i]; //  +  errInt[i];

        // Update ship kinematics: s1 = s0 + v0*t + 0.5*a*t^2;  v1 = v0 + a*t;
        shipAccel[i] = shipForce[i]/SHIPMASS;
        shipPos[i] += shipVel[i] + 0.5 * shipAccel[i];
        shipVel[i] += shipAccel[i];

    }


    // test stuff for kuka in gazebo by printing out plots
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
    
    timeOut = 10;
    // Broadcast to yellowjackets
    MPI_Bcast(&timeOut, 1, MPI_INT, root, MPI_COMM_WORLD);
    MPI_Bcast(&maxThrust, 1, MPI_DOUBLE, root, MPI_COMM_WORLD);
    MPI_Bcast(allShipInfo, 56, MPI_DOUBLE, root, MPI_COMM_WORLD);

    // set the initial ship velocity for all ships from allShipInfo
    for (int i = 0; i < 3; ++i)
    {
        shipPos[i] = allShipInfo[7*rank + i];
        shipVel[i] = allShipInfo[7*rank + 3] * allShipInfo[7*rank + 3 + i + 1]; 
    }
 
    
    // Loop through the number of time steps
    for (int round = 0; round < timeOut; ++round)
    {
        // Share information with other yellow jackets
        MPI_Allgather(&shipStatus, 1, MPI_INT, &recvAllShipStatus, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Allgather(shipPos, posLen, MPI_DOUBLE, recvAllShipPos, posLen, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgather(shipVel, velLen, MPI_DOUBLE, recvAllShipVel, velLen, MPI_DOUBLE, MPI_COMM_WORLD);
        MPI_Allgather(shipForce, forceLen, MPI_DOUBLE, recvAllShipForce, forceLen, MPI_DOUBLE, MPI_COMM_WORLD);
            
        if (rank == 0)
        {
            // Calculate Buzzy new location
            CalculateBuzzyXYZ();

            // Output all ship info to console:  rankID, status, x, y, z, F x , F y , F z
            std::cout << std::endl;
            std::cout << "------------- Round " << round << " -------rankID, status, x, y, z, Fx , Fy , Fz --------------" << std::endl;
            // debug print out
            for (int i = 0; i < numtasks; ++i)
            {
                std::cout << i << " " << recvAllShipStatus[i] << " ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << recvAllShipPos[3*i + j] << " ";
                }
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << recvAllShipVel[3*i + j] << " ";
                }
                std::cout << std::endl;
            }
            /*
            for (int i = 1; i < numtasks; ++i)
            {
                std::cout << i << " " << recvAllShipStatus[i] << " ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipPos[3*i + j] << " ";
                }
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipForce[3*i + j] << " ";
                }
                std::cout << std::endl;
            }
            */
        }
        else
        {
            // Calculate yellow jacket new location
            CalculateYellowJacketXYZ();
        }

        for (int i = 1; i < numtasks; ++i)
        {
            if (recvAllShipStatus[i] == 0)
            {
                shipDestroyed = true;
                break;
            }
            else if (recvAllShipStatus[i] == 1)
            {
                allShipsDocked = false;
                break;
            }
        }
        
        if (shipDestroyed)
        {
            break;
        }
        else if (allShipsDocked)
        {
            break;
        }
           
    }

    if (rank == 0)
    {
        for (int i = 1; i < numtasks; ++i)
        {
            if (recvAllShipStatus[i] == 1)
            {
                std::cout << "\n" << "Timeout! Some Yellow Jackets were unable to dock with Buzzy within the time limit! \n\n";
                break;
            }
        }
    }
    
    MPI_Finalize();
}
