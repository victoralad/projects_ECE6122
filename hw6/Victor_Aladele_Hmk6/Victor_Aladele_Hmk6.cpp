/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Nov 12, 2019
Description:
    3D chess set model using OpenGL
*/

#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions



#define ESC 27

GLfloat light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat light0_specular[] = {0.8, 0.8, 0.8, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light1_position[] = {-5.0, -5.0, 8.0};

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL); 
    
    glEnable(GL_DEPTH_TEST);
}

//----------------------------------------------------------------------
// Global variables
//
// The coordinate system is set up so that the (x,y)-coordinate plane
// is the ground, and the z-axis is directed upwards. The y-axis points
// to the north and the x-axis points to the east.
//
// The values (x,y, z) are the current camera position. The values (lx, ly, lz)
// point in the direction the camera is looking. The variables angle and
// deltaAngle control the camera's angle. The variable deltaMove
// indicates the amount of incremental motion for the camera with each
// redraw cycle. 
//----------------------------------------------------------------------

// Camera position
float x = 4.0, y = -5.0, z = 10; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 4.0, ly = 4.0, lz = 0.0; 
float angle = 0.0; // angle of rotation for the camera direction

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
    gluPerspective(42.0, ratio, 0.1, 30.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

//----------------------------------------------------------------------
// Draw chess pieces (starting at the origin)
// White pieces drawn first followed by black pieces
// ----------------------------------------------------------------------
void drawChessPieces()
{
    int i;
    // ----------- Draw white chess pieces ----------- 
    // Draw pawn pieces
    for (i = 0; i < 8; ++i) {
        glColor3f(0.549, 0.549, 0.529); // set drawing color to white
        glPushMatrix();
            glTranslatef(i + 0.5, 1.5, 0.375);
            glutSolidSphere(0.375, 20, 20);
        glPopMatrix();
    }

    // ----------- Draw black chess pieces ----------- 
    // Draw pawn pieces
    for (i = 0; i < 8; ++i) {
        glColor3f(0.588, 0.294, 0); // set drawing color to black
        glPushMatrix();
            glTranslatef(i + 0.5, 6.5, 0.375);
            glutSolidSphere(0.375, 20, 20);
        glPopMatrix();
    }
}

//----------------------------------------------------------------------
// Update with each idle event
//
// This incrementally moves the camera and requests that the scene be
// redrawn.
//----------------------------------------------------------------------
void update(void)
{
    if (deltaMove) { // update camera position
        x += deltaMove * lx * 0.1;
        y += deltaMove * ly * 0.1;
    }
    glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// Draw the entire scene
//
// We first update the camera location based on its distance from the
// origin and its direction.
//----------------------------------------------------------------------
void renderScene(void)
{
    int i, j;

     // Clear color and depth buffers
    glClearColor(0.1, 0.3, 0.1, 1.0); // background color is green
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Set the camera centered at (x,y,z) and looking along directional
    // vector (lx, ly, lz), with the z-axis pointing up
    gluLookAt(
        x, y, z,
        lx, ly, lz,
        0.0, 1.0, 0.0);

    // draw chess board
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if ((i + j) % 2 == 0) 
            {
                // draw black boxes on chess board
                glColor3f(0.0, 0.0, 0.0);
                glPushMatrix();
                    glTranslatef(i, j, 0);
                    glBegin(GL_QUADS);
                        glVertex3f(0.0, 0.0, 0.0);
                        glVertex3f(0.0, 1.0, 0.0);
                        glVertex3f(1.0, 1.0, 0.0);
                        glVertex3f(1.0, 0.0, 0.0);
                    glEnd();
                glPopMatrix();
            }
            
            else 
            {
                // draw white boxes on chess board
                glColor3f(1.0, 1.0, 1.0);
                glPushMatrix();
                    glTranslatef(i, j, 0);
                    glBegin(GL_QUADS);
                        glVertex3f(0.0, 0.0, 0.0);
                        glVertex3f(0.0, 1.0, 0.0);
                        glVertex3f(1.0, 1.0, 0.0);
                        glVertex3f(1.0, 0.0, 0.0);
                    glEnd();
                glPopMatrix();
            }
        }
    }

    drawChessPieces();

    glutSwapBuffers(); // Make it all visible
}

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
    if (key == ESC || key == 'q' || key == 'Q')
    {
        exit(0);
    }
}

void pressSpecialKey(int key, int xx, int yy)
{
    switch (key) 
    {
    case GLUT_KEY_UP: deltaMove = 1.0; break;
    case GLUT_KEY_DOWN: deltaMove = -1.0; break;
    }
}

void releaseSpecialKey(int key, int x, int y)
{
    switch (key) 
    {
    case GLUT_KEY_UP: deltaMove = 0.0; break;
    case GLUT_KEY_DOWN: deltaMove = 0.0; break;
    }
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
    printf("\n\
-----------------------------------------------------------------------\n\
  OpenGL Sample Program:\n\
  - Hold up-arrow/down-arrow to move camera forward/backward\n\
  - q or ESC to quit\n\
-----------------------------------------------------------------------\n");

    // general initializations
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Chess Set");
    init();
    // register callbacks
    glutReshapeFunc(changeSize); // window reshape callback
    glutDisplayFunc(renderScene); // (re)display callback
    glutIdleFunc(update); // incremental update
    glutIgnoreKeyRepeat(1); // ignore key repeat when holding key down
    glutKeyboardFunc(processNormalKeys); // process standard key clicks
    glutSpecialFunc(pressSpecialKey); // process special key pressed
                        // Warning: Nonstandard function! Delete if desired.
    glutSpecialUpFunc(releaseSpecialKey); // process special key release


    // enter GLUT event processing cycle
    glutMainLoop();

    return 0; // this is just to keep the compiler happy
}