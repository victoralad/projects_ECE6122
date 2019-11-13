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
// Draw one snowmen (at the origin)
//
// A snowman consists of a large body sphere and a smaller head sphere.
// The head sphere has two black eyes and an orange conical nose. To
// better create the impression they are sitting on the ground, we draw
// a fake shadow, consisting of a dark circle under each.
//
// We make extensive use of nested transformations. Everything is drawn
// relative to the origin. The snowman's eyes and nose are positioned
// relative to a head sphere centered at the origin. Then the head is
// translated into its final position. The body is drawn and translated
// into its final position.
//----------------------------------------------------------------------
// void drawSnowman()
// {
//     // Draw body (a 20x20 spherical mesh of radius 0.75 at height 0.75)
//     glColor3f(1.0, 1.0, 1.0); // set drawing color to white
//     glPushMatrix();
//         glTranslatef(0.0, 0.0, 0.75);
//         glutSolidSphere(0.75, 20, 20);
//     glPopMatrix();

//     // Draw the head (a sphere of radius 0.25 at height 1.75)
//     glPushMatrix();
//         glTranslatef(0.0, 0.0, 1.75); // position head
//         glutSolidSphere(0.25, 20, 20); // head sphere

//         // Draw Eyes (two small black spheres)
//         glColor3f(0.0, 0.0, 0.0); // eyes are black
//         glPushMatrix();
//             glTranslatef(0.0, -0.18, 0.10); // lift eyes to final position
//             glPushMatrix();
//                 glTranslatef(-0.05, 0.0, 0.0);
//                 glutSolidSphere(0.05, 10, 10); // right eye
//             glPopMatrix();
//             glPushMatrix();
//                 glTranslatef(+0.05, 0.0, 0.0);
//                 glutSolidSphere(0.05, 10, 10); // left eye
//             glPopMatrix();
//         glPopMatrix();

//         // Draw Nose (the nose is an orange cone)
//         glColor3f(1.0, 0.5, 0.5); // nose is orange
//         glPushMatrix();
//             glRotatef(90.0, 1.0, 0.0, 0.0); // rotate to point along -y
//             glutSolidCone(0.08, 0.5, 10, 2); // draw cone
//         glPopMatrix();
//     glPopMatrix();

//     // Draw a faux shadow beneath snow man (dark green circle)
//     glColor3f(0.0, 0.5, 0.0);
//     glPushMatrix();
//         glTranslatef(0.2, 0.2, 0.001);    // translate to just above ground
//         glScalef(1.0, 1.0, 0.0); // scale sphere into a flat pancake
//         glutSolidSphere(0.75, 20, 20); // shadow same size as body
//     glPopMatrix();
// }

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
    glClearColor(0.0, 0.7, 1.0, 1.0); // sky color is light blue
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Set the camera centered at (x,y,z) and looking along directional
    // vector (lx, ly, lz), with the z-axis pointing up
    gluLookAt(
        x, y, z,
        lx, ly, lz,
        0.0, 1.0, 0.0);

    // Draw ground - 8 x 8 grid chess board 
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 8.0, 0.0);
        glVertex3f(8.0, 8.0, 0.0);
        glVertex3f(8.0, 0.0, 0.0);
    glEnd();

    // Draw 16 white boxes on chess board
    glColor3f(1.0, 1.0, 1.0);
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if ((i + j) % 2 != 0) 
            {
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
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Chess Set");
    glEnable(GL_COLOR_MATERIAL); 
    // glEnable(GL_DEPTH_TEST);
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