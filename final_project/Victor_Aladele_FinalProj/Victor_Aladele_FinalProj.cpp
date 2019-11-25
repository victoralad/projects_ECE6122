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
#include "ECE_Bitmap.h"
#include <chrono>
#include <thread>

#define ESC 27
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
GLfloat light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat light0_diffuse[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light0_specular[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_diffuse[] = {0.5, 0.5, 0.5, 1.0};
GLfloat light1_specular[] = {0.3, 0.3, 0.3, 1.0};
GLfloat mat_specular[] = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light1_position[] = {5.0, 5.0, 8.0};

// texturing variables
GLuint texture[1];
BMP inBitmap;

// Send location and velocity vector in each direction
const int numElements = 6; // x, y, z, vx, vy, vz

const int rcvSize = 16 * 6; // (Main task + 15 UAVs) * numElements

double* rcvbuffer = new double[rcvSize];

double sendBuffer[numElements];

void init(void)
{
    // turn on light and material functions
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

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
    inBitmap.read("WinterIsland.bmp");

    // makeCheckImage();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Create Textures

    glGenTextures(1, texture);
    
    // Setup first texture
    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //scale linearly when image bigger than texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //scale linearly when image smalled than texture


    glTexImage2D(GL_TEXTURE_2D, 0, 3, inBitmap.bmp_info_header.width, inBitmap.bmp_info_header.height, 0,
        GL_BGR_EXT, GL_UNSIGNED_BYTE, &inBitmap.data[0]);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

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
    gluPerspective(70.0, ratio, 0.1, 100.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

void displayFootballField()
{

    glColor3f(0.1, 0.6, 0.1);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_QUADS);
            glTranslatef(0, 0, 0.0);
            glVertex3f(0.0, 0.0, 0.0);
            glVertex3f(0.0, width + 2 * bounds, 0.0);
            glVertex3f(length + 2 * bounds, width + 2 * bounds, 0.0);
            glVertex3f(length + 2 * bounds, 0.0, 0.0);
        glEnd();
    glPopMatrix();

    // glColor3f(0.0, 0.9, 0.0);
    // glPushMatrix();
    //     glBegin(GL_QUADS);
    //         glVertex3f(bounds, bounds, 0.0);
    //         glVertex3f(bounds, width + bounds, 0.0);
    //         glVertex3f(length + bounds, width + bounds, 0.0);
    //         glVertex3f(length + bounds, bounds, 0.0);
    //     glEnd();
    // glPopMatrix();

}

void drawUAVs()
{
    glColor3f(0.8, 0.1, 0.1);
    for (float i = 0; i <= width; i+= width / 2)
    {
        for (float j = 9.144; j <= length; j+= (length - 18.288) / 4)
        {
            glPushMatrix();
                glTranslatef(j + bounds, i + bounds, 0);
                glutSolidCone(0.5, 1.0, 20, 20);
            glPopMatrix();
        }
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

    glBindTexture(GL_TEXTURE_2D, texture[0]);

    glPushMatrix();
    glTranslatef(55, 24.5, 0);
    glScalef(55, 24.5, 0);
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

    displayFootballField();

    drawUAVs();

    glutSwapBuffers(); // Make it all visible

    MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
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

//----------------------------------------------------------------------
// mainOpenGL  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
void timer(int id)
{
    glutPostRedisplay();
    glutTimerFunc(100, timer, 0);
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

    // Setup lights as needed
    // ...

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
        for (int ii = 0; ii < 600 ; ii++)
        {
            // CalcualteUAVsLocation(rank); 
            MPI_Allgather(sendBuffer, numElements, MPI_DOUBLE, rcvbuffer, numElements, MPI_DOUBLE, MPI_COMM_WORLD);
        }
    }
    return 0;
}