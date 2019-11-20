/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Nov 05, 2019
Description:
    A distributed MPI program to simulate the docking of 
    spacehips (Yellow Jackets) with the mothership (Buzzy)
    The status of all ships is continually updated and monitored
    Active = 1, Docked = 2, Destroyed = 0
*/

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
double posGainsToBuzzy[8] = {0, 300, 100, 200, 50, 150, 30, 10}; // position control gains to drive the each ship towards Buzzy
double velGainsToBuzzy[8] = {0, 300, 300, 200, 100, 200, 30, 10};

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


void CalculateBuzzyXYZ();

void CalculateYellowJacketXYZ();

void updateShipStatus(double distSqToAllShips[], double distSqToBuzzy);

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
    srand(time(NULL));  // seed the random number generator

    double errPos[posLen] = {0};
    double errVel[velLen] = {0};
    double errInt[3] = {0}; // Integral error that is proportional to the velocity of Buzzy
    double shipAccel[3] = {0};

    // calculate distance to Buzzy
    double distSqToBuzzy = 0;
    for (int i = 0; i < posLen; ++i) 
    {
        distSqToBuzzy += (recvAllShipPos[i] - shipPos[i]) * (recvAllShipPos[i] - shipPos[i]);
    }

    // calculate distance to every other ship 
    double distSqToAllShips[numtasks - 1] = {0};
    for (int i = 1; i < numtasks; ++i) 
    {
        for (int j = 0; j < posLen; ++j) 
        {
            distSqToAllShips[i - 1] += (recvAllShipPos[posLen*i + j] - shipPos[j]) * (recvAllShipPos[posLen*i + j] - shipPos[j]);
        }
    }

    // use PD controller  // note time = 1 second
    for (int i = 0; i < 3; ++i)
    {
        double mKp = posGainsToBuzzy[rank] / 5; // PD controller gains
        double mKd = velGainsToBuzzy[rank] / 5; 

        errPos[i] = recvAllShipPos[i] - shipPos[i];
        errVel[i] = recvAllShipVel[i] - shipVel[i];
        errInt[i] = recvAllShipVel[i];
        shipForce[i] = 0; //mKp * errPos[i]  +  mKd * errVel[i]; //  +  errInt[i];

        // Update ship kinematics: s1 = s0 + v0*t + 0.5*a*t^2;  v1 = v0 + a*t;
        shipAccel[i] = (rand()%40 + 80) / 100.0 * shipForce[i]/SHIPMASS; // added some randomness to simulate misfiring of the thrusters 
        shipPos[i] += shipVel[i] + 0.5 * shipAccel[i];
        shipVel[i] += shipAccel[i];
    }

    updateShipStatus(distSqToAllShips, distSqToBuzzy);
}

void updateShipStatus(double distSqToAllShips[], double distSqToBuzzy)
{
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

    for (int i = 0; i < numtasks - 1; ++i) 
    {
        if (i + 1 == rank)
        {
            continue;
        }
        // check if ship is at a safe distance from every other ship
        if (distSqToAllShips[i] < 250 * 250)
        {
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
    if (rank == 0)
    {
        // Load in.dat file
        readInputData();
    }
    
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
            for (int i = 1; i < numtasks; ++i)
            {
                std::cout << i << ", " << recvAllShipStatus[i] << ", ";
                for (int j = 0; j < 3; ++j)
                {
                    std::cout << std::scientific << recvAllShipPos[posLen * i + j] << ", ";
                }
                for (int j = 0; j < 2; ++j)
                {
                    std::cout << std::scientific << recvAllShipForce[forceLen * i + j] << ", ";
                }
		        std::cout << std::scientific << recvAllShipForce[forceLen * i + 2] << std::endl;
            }
        }
        else
        {
            // Calculate yellow jacket new location
            if (shipStatus == 1)
                CalculateYellowJacketXYZ();
            }
        }
    
    MPI_Finalize();
}