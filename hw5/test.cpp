// Demonstrate simple MPI program
// This one uses non-blocking ISend/Irecv
// George F. Riley, Georgia Tech, Fall 2011


#include <iostream>
#include <ctime>
#include <cmath> 
#include <fstream>
#include <mpi.h>


#define SHIPMASS 10000;

int numtasks, rank, rc;
const int root = 0;

int timeOut; 
double maxThrust;
double allShipInfo[56] = {0};
double posGains[7] = {300, 300, 200, 100, 200, 30, 10}; 
double velGains[7] = {300, 300, 200, 100, 200, 30, 10};

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


double mKp = posGains[rank] / 5, mKd = velGains[rank] / 5; // PD controller gains
double errPos[posLen] = {0};
double errVel[velLen] = {0};
double errInt[3] = {0}; // Integral error that is proportional to the velocity of Buzzy
double shipAccel[3] = {0};

void CalculateBuzzyXYZ();

void CalculateYellowJacketXYZ();

void updateShipStatus();

void readInputData();

void CalculateBuzzyXYZ()
{
    // calculate new position of Buzzy using Newton's equation s1 = s0 + v*t
    shipPos[0] += shipVel[0]; // x
    shipPos[1] += shipVel[1]; // y
    shipPos[2] += shipVel[2]; // z
}

void CalculateYellowJacketXYZ()
{
    srand(time(NULL));

    // use PD controller  // note time = 1 second
    for (int i = 0; i < 3; ++i)
    {
        errPos[i] = recvAllShipPos[i] - shipPos[i];
        errVel[i] = recvAllShipVel[i] - shipVel[i];
        errInt[i] = recvAllShipVel[i];
        shipForce[i] = mKp * errPos[i]  +  mKd * errVel[i];//  +  errInt[i];

        // Update ship kinematics: s1 = s0 + v0*t + 0.5*a*t^2;  v1 = v0 + a*t;
        shipAccel[i] = (rand()%40 + 80) / 100.0 * shipForce[i]/SHIPMASS; // added some randomness to simulate misfiring of the thrusters 
        shipPos[i] += shipVel[i] + 0.5 * shipAccel[i];
        shipVel[i] += shipAccel[i];
    }

    updateShipStatus();
}

void updateShipStatus()
{
    // calculate distance to Buzzy
    double distSqToBuzzy = 0;
    for (int i = 0; i < posLen; ++i) 
    {
        distSqToBuzzy += (recvAllShipPos[i] - shipPos[i]) * (recvAllShipPos[i] - shipPos[i]);
    }

    // calculate velocity magnitude of Buzzy
    double yJacVelMag = sqrt(shipVel[0]*shipVel[0] + shipVel[1]*shipVel[1] + shipVel[2]*shipVel[2]);
    double dotProduct = shipVel[0]*recvAllShipVel[0] + shipVel[1]*recvAllShipVel[1] + shipVel[2]*recvAllShipVel[2];
    
    // calculate orientation of yellow Jacket with respect to Buzzy
    double cosTheta = dotProduct / (yJacVelMag * allShipInfo[3]);

    // calculate velocity magnitude of Buzzy
    double buzzyVelMag = sqrt(recvAllShipVel[0]*recvAllShipVel[0] + recvAllShipVel[1]*recvAllShipVel[1] + recvAllShipVel[2]*recvAllShipVel[2]);

    // check if all necessary conditions for docking
    if (distSqToBuzzy < 50 * 50)
    {
        if (cosTheta <= 0.8 || yJacVelMag >= 1.1 * buzzyVelMag) 
        {
            shipStatus = 0; // ship crashed!!!
        }
        else
        {
            shipStatus = 2; // ship docked!
        }
        
    }

    // calculate distance to every other ship 
    double distSqToAllShips[6] = {0};
    for (int i = 1; i < numtasks; ++i) 
    {
        if (i == rank)
        {
            continue;
        }
        for (int j = 0; j < posLen; ++j) 
        {
            distSqToAllShips[i - 1] += (recvAllShipPos[posLen*i + j] - shipPos[j]) * (recvAllShipPos[posLen*i + j] - shipPos[j]);
        }

        // check if ship is at a safe distance from every other ship
        if (distSqToAllShips[i - 1] < 250 * 250)
        {
            std::cout << "ship - " << rank << " collided with ship: " << i << " at position " << distSqToAllShips[i - 1] << std::endl;
            shipStatus = 0; // ship crashed!!!
            break;
        }
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
    
    timeOut = 3000;
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
                std::cout << i << ", " << recvAllShipStatus[i] << ", ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << recvAllShipPos[posLen * i + j] << ", ";
                }
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << recvAllShipVel[velLen*i + j] << ", ";
                }
                std::cout << std::endl;
            }
            /*
            for (int i = 1; i < numtasks; ++i)
            {
                std::cout << i << ", " << recvAllShipStatus[i] << ", ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipPos[posLen * i + j] << ", ";
                }
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipForce[forceLen * i + j] << ", ";
                }
                std::cout << std::endl;
            }
            */
        }
        else
        {
            // Calculate yellow jacket new location
            if (shipStatus == 1)
            {
                CalculateYellowJacketXYZ();
            }
        }

        bool allShipsDestroyed = true;
        bool allShipsDocked = true;

        for (int i = 1; i < numtasks; ++i)
        {
            if (recvAllShipStatus[i] == 0)
            {
                allShipsDestroyed = true &&  allShipsDestroyed;
            }
            else
            {
                allShipsDestroyed = false;
            }
            
            if (recvAllShipStatus[i] == 2)
            {
                allShipsDocked = true && allShipsDocked;
            }
            else
            {
                allShipsDocked = false;
            }
            
        }
        
        if (allShipsDestroyed)
        {
            if (rank == 0)
            {
                std::cout << "\n" << "Mission failed! All Yellow Jackets have been destroyed! \n\n";
            }
            break;
        }
        else if (allShipsDocked)
        {
            if (rank == 0)
            {
                std::cout << "\n" << "Success! All Yellow Jackets have docked with Buzzy at time: " << round + 1 << " seconds! \n\n";
            }
            break;
        }
        // MPI_Barrier(MPI_COMM_WORLD);
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
