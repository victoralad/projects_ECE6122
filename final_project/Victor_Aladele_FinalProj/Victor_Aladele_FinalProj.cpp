/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Nov 15, 2019
Description:
    3D simulation of a half time UAV show using MPI and OpenGL.
    The UAVs are red in color and spherical in shape. 
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "iomanip"
#include <cmath>
#include <math.h>
#include <cstdlib>
#include <GL/glut.h>
#include "Victor_Aladele_FinalProj.h"
#include <chrono>
#include <thread>

#define ESC 27
#define gravity -10 
#define mass 1 // mass of a UAV in kg
//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y, z) are the current camera position. The values (lx, ly, lz)
// point in the direction the camera is looking. 
//----------------------------------------------------------------------

float angle = 0.0; // initial orientation of the chessboard
float length = 110.0, width = 49.0;
float bounds = 2.0;

// Camera position
float eyeX = length / 2, eyeY = -width / 3.0, eyeZ = 75; // initially 5 units south of origin
float deltaMoveY = 0.0; // initially camera doesn't move
float deltaMoveZ = 0.0;

// Camera direction
float lx = length / 2, ly = width / 2, lz = 0.0; 

// initialize lighting values and material properties
GLfloat light0_ambient[] = {0.3, 0.3, 0.3, 1.0};
GLfloat light0_diffuse[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light0_specular[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_diffuse[] = {0.5, 0.5, 0.5, 1.0};
GLfloat light1_specular[] = {0.3, 0.3, 0.3, 1.0};
GLfloat mat_ambient[] = {0.8, 0.8, 0.8, 1.0};
GLfloat mat_specular[] = {0.8, 0.8, 0.8, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light1_position[] = {55.0, 25.0, 70.0};

// texturing variables
GLuint texture[1];
BMP inBitmap;

// Send location and velocity vector in each direction
const int sendSize = 6; // x, y, z, vx, vy, vz
const int recvSize = 16 * 6; // (Main task + 15 UAVs) * sendSize
double recvBuffer[recvSize] = {0};
double sendBuffer[sendSize] = {0};
double accel[3] = {0};
double force[3] = {0};
double gravForce[3] = {0, 0, gravity};
double kP[3] = {0.5, 0.5, 0.8}; // position control gain
double kV[3] = {0.5, 0.5, 0.5}; // velocity control gain
double goal[3] = {0, 0, 10}; // target position

void init()
{
    // turn on light and material functions
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL); 
    
    glEnable(GL_DEPTH_TEST);

    /* initialize random seed: */
    srand(time(NULL));

    // texturing
    inBitmap.read("ff.bmp");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create Textures

    glGenTextures(1, texture);
    
    // Setup texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture

    glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);

    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,  GL_DECAL);

    glEnable(GL_TEXTURE_2D);
}

//----------------------------------------------------------------------
// Reshape callback
//
// Window size has been set/changed to w by h pixels. Set the camera
// perspective to 45 degree vertical field of view, a window aspect
// ratio of w/h, a near clipping plane at depth 1, and a far clipping
// plane at depth 100. The viewport is the entire window.
//
//----------------------------------------------------------------------
void changeSize(int w, int h)
{
    float ratio = ((float)w) / ((float)h); // window aspect ratio
    glMatrixMode(GL_PROJECTION); // projection matrix is active
    glLoadIdentity(); // reset the projection
    gluPerspective(80.0, ratio, 0.1, 100.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

void displayFootballField()
{
    glColor3f(0.9, 0.9, 0.9);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glPushMatrix();
        glTranslatef(55 + bounds, 24.5 + bounds, 0);
        glScalef(55 + 1.5*bounds, 24.5 + 1.5*bounds, 0);
        glBegin(GL_QUADS);
            glTexCoord2f(1, 1);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(0, 1);
            glVertex3f(-1.0f, 1.0f, 0.0f);
            glTexCoord2f(0, 0);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(1, 0);
            glVertex3f(1.0f, -1.0f, 0.0f);
        glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D,0);
}

void drawUAVs()
{
    glColor3f(1.0, 0.0, 0.0);
    int rankID = 1;
    std::cout << " -------drawUAVs --------" << std::endl;
    for (float i = 0; i <= width; i+= width / 2)
    {
        for (float j = 9.144; j <= length; j+= (length - 18.288) / 4)
        {
            std::cout << recvBuffer[sendSize * rankID + 5] << " ";
            glPushMatrix();
                glTranslatef(j + bounds, i + bounds, recvBuffer[sendSize * rankID + 2]);
                glutSolidCone(1, 2.0, 20, 20);
            glPopMatrix();
            
            rankID++;
        }
        std::cout << std::endl;
    }
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene()
{

    // Clear color and depth buffers
    glClearColor(0.7, 0.8, 1.0, 1.0); // background color is blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    gluLookAt(eyeX, eyeY, eyeZ, 
              lx, ly, lz,
              0.0, 0.0, 1.0);

    glMatrixMode(GL_MODELVIEW);

    displayFootballField();

    drawUAVs();

    glutSwapBuffers(); // Make it all visible

    MPI_Allgather(sendBuffer, sendSize, MPI_DOUBLE, recvBuffer, sendSize, MPI_DOUBLE, MPI_COMM_WORLD);

    // std::cout << " --######## mpi #############---" << std::endl;
    // for (int rankID = 1; rankID < 16; ++rankID) 
    // {
    //     for (int i = 0; i < 6; ++i)
    //     {
    //         std::cout << recvBuffer[6 * rankID + i] << " ";
    //     }
    //     std::cout << std::endl;
    // }
}

void update()
{
    if (deltaMoveY)
    {
        eyeY += deltaMoveY;
        deltaMoveY = 0.0;
    }
    if (deltaMoveZ)
    {
        eyeZ += deltaMoveZ;
        deltaMoveZ = 0.0;
    }
}

// debug function to move field around
void processNormalKeys(unsigned char key, int xx, int yy)
{
    switch (key)
    {
        case ESC : exit(0); break;
        case 'q' : exit(0); break;
        case 'Q' : exit(0); break;
        case '1' : deltaMoveY = 0.25; break;
        case '0' : deltaMoveY = -0.25; break;
        case 'u' : deltaMoveZ = 0.25; break;
        case 'd' : deltaMoveZ = -0.25; break;
    }
}

// update the state of each UAV
void calculateUAVsLocation(int rank)
{
    // update velocity
    for(int i = 0; i < 3; ++i) 
    {
        // calculate force
        force[i] = kP[i] * (goal[i] - sendBuffer[i]) - kV[i] * sendBuffer[i + 3];
        force[i] = std::min(20.0, std::max(-20.0, force[i]));

        // calculate acceleration
        accel[i] = (force[i] + gravForce[i]) / mass;

        // update velocity
        sendBuffer[i + 3] = sendBuffer[i + 3] + accel[i];

        // update position
        sendBuffer[i] = sendBuffer[i] + sendBuffer[i + 3] + 0.5 * accel[i];

        // std::cout << " --######## mpi " << rank << " #############---" << std::endl;
        // for (int i = 0; i < 6; ++i)
        // {
        //     std::cout << sendBuffer[i] << " ";
        // }
        // std::cout << std::endl;
    }
}

//----------------------------------------------------------------------
// timerFunction  - called whenever the timer fires
//----------------------------------------------------------------------
void timerFunc(int id)
{
    glutPostRedisplay();
    glutTimerFunc(100, timerFunc, 0);
}

//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void mainOpenGL(int argc, char**argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(400, 400);
    glutCreateWindow("Super Bowl half-time Show");
    init();

    glutReshapeFunc(changeSize);
    glutDisplayFunc(renderScene);
    glutIdleFunc(update); // incremental update
    glutKeyboardFunc(processNormalKeys); // process standard key clicks
    glutTimerFunc(100, timerFunc, 0);
    glutMainLoop();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Main entry point determines rank of the process and follows the 
// correct program path
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
int main(int argc, char**argv)

{
    int numTasks, rank;

    int rc = MPI_Init(&argc, &argv);

    if (rc != MPI_SUCCESS) 
    {
        printf("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &numTasks);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int gsize = 0;

    MPI_Comm_size(MPI_COMM_WORLD, &gsize);


    if (rank == 0) 
    {
        mainOpenGL(argc, argv);
    }
    else
    {
        // Sleep for 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
        for (int i = 0; i < 600 ; ++i)
        {
            calculateUAVsLocation(rank); 
            MPI_Allgather(sendBuffer, sendSize, MPI_DOUBLE, recvBuffer, sendSize, MPI_DOUBLE, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
    // return 0;
}