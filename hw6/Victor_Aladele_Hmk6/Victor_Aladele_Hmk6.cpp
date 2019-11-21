/*
Author: Victor Aladele
Class: ECE6122
Last Date Modified: Nov 20, 2019
Description:
    3D chess set model using OpenGL
*/

#include <iostream>
#include <GL/glut.h> // Include the GLUT header file
#include <stdio.h>
#include <math.h>
#include <stdlib.h> // standard definitions
#include <vector>
#include <ctime>



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

// Camera position
float x = 4.0, y = -5.0, z = 10; // initially 5 units south of origin
float deltaMove = 0.0; // initially camera doesn't move

// Camera direction
float lx = 4.0, ly = 4.0, lz = 0.0; 

float angle = 0.0; // initial orientation of the chessboard

GLfloat light0_ambient[] = {0.2, 0.2, 0.2, 1.0};
GLfloat light0_diffuse[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light0_specular[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_ambient[] = {0.0, 0.0, 0.0, 0.0};
GLfloat light1_diffuse[] = {0.5, 0.5, 0.5, 1.0};
GLfloat light1_specular[] = {0.3, 0.3, 0.3, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_shininess[] = {50.0};
GLfloat light1_position[] = {5.0, 5.0, 8.0};
GLdouble width = 0.75, depth = 0.75, height = 1.0;
bool light0Enable = true, light1Enable = true;

std::vector<std::vector<int> > freeSpot(8, std::vector<int> (8, 1));
std::vector<int> whitePawnY(8, 1); // y positions of white pawns
std::vector<int> blackPawnY(8, 6);
std::vector<int> knightsX{1, 6, 1, 6}; // x positions of knights (white knights first)
std::vector<int> knightsY{0, 0, 7, 7};
bool rotateBoard = false;
bool movePawn = false;
bool moveKnight = false;

void init(void)
{
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
    gluPerspective(43, ratio, 0.1, 30.0); // perspective transformation
    glMatrixMode(GL_MODELVIEW); // return to modelview mode
    glViewport(0, 0, w, h); // set viewport (drawing area) to entire window
}

// print state of the chessboard (debug function)
void printFreeSpot()
{
    std::cout << "------- State of Chessboard (black->top, white->bottom) -------" << std::endl;
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            std::cout << freeSpot[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

// Update information about a spot; whether it is free or occupied
void updateSpotInfo(int y, int x, int openSpot)
{
    freeSpot[7 - x][y] = openSpot;
}

// Move knights
void moveKnightFunc()
{
    int count = 0;
    while (count < 4)
    {
        int i = rand() % 4;
        std::cout << i << std::endl;
        if (7 - (knightsY[i] + 2) >= 0 && knightsX[i] - 1 >= 0 && freeSpot[7 - (knightsY[i] + 2)][knightsX[i] - 1])
        {
            std::cout << i << " " << 7 - (knightsY[i] + 2) << " " << knightsX[i] - 1 << std::endl;
            freeSpot[7 - knightsY[i]][knightsX[i]] = 1;
            knightsY[i] = (knightsY[i] + 2) % 8;
            knightsX[i] = (knightsX[i] - 1) % 8;
            freeSpot[7 - knightsY[i]][knightsX[i]] = 0;
            break;
        }
        // int getRandNum = rand() % 4;
        // int knightColor = getRandNum / 2;
        // int i = getRandNum;// % 2;
        // std::cout << i << std::endl;
        // // for (int j = 0; j < 8; ++j)
        // // {
        //     // move white knight
        //     if (freeSpot[7 - (knightsY[i] + 2) % 8][7 - (knightsX[i] - 1) % 8] && knightColor == 0)
        //     {
        //         freeSpot[7 - knightsY[i]][knightsX[i]] = 1;
        //         knightsY[i] = (knightsY[i] + 2) % 8;
        //         knightsX[i] = (knightsX[i] - 1) % 8;
        //         freeSpot[7 - knightsY[i]][knightsX[i]] = 0;
        //         break;
        //     }
        //     // move black knight
        //     else if (freeSpot[7 - (knightsY[i] - 2) % 8][7 - (knightsX[i] - 1) % 8] && knightColor == 1)
        //     {
        //         freeSpot[7 - knightsY[i]][knightsX[i]] = 1;
        //         knightsY[i] = (knightsY[i] - 2) % 8;
        //         knightsX[i] = (knightsX[i] - 1) % 8;
        //         freeSpot[7 - knightsY[i]][knightsX[i]] = 0;
        //         break;
        //     }
        // }
        count++;
    }
}

// Move pawn forward
void movePawnFunc()
{
    int count = 0;
    while (count < 16)
    {
        int getRandNum = rand() % 16;
        int pawnColor = getRandNum / 8;
        int i = getRandNum % 8;

        // move white pawn forward
        if (freeSpot[7 - (whitePawnY[i] + 1) % 8][i] && pawnColor == 0)
        {
            freeSpot[7 - whitePawnY[i]][i] = 1;
            whitePawnY[i] = (whitePawnY[i] + 1) % 8;
            freeSpot[7 - whitePawnY[i]][i] = 0;
            break;
        }
        // move black pawn forward
        else if (freeSpot[7 - (blackPawnY[i] - 1) % 8][i] && pawnColor == 1)
        {
            freeSpot[7 - blackPawnY[i]][i] = 1;
            blackPawnY[i] = (blackPawnY[i] - 1) % 8;
            freeSpot[7 - blackPawnY[i]][i] = 0;
            break;
        }
        count++;
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
    if (rotateBoard)
    {
        // rotate chessboard
        angle += 10.0;
        rotateBoard = false;
    }

    if (deltaMove) 
    { 
        // update camera position
        z += deltaMove;
    }

    light0Enable ? glEnable(GL_LIGHT0) : glDisable(GL_LIGHT0);

    light1Enable ? glEnable(GL_LIGHT1) : glDisable(GL_LIGHT1); 

    if (moveKnight) 
    {
        moveKnightFunc();
        moveKnight = false;
    }

    if (movePawn) 
    {
        movePawnFunc();
        movePawn = false;
    }

    glutPostRedisplay(); // redisplay everything
}

//----------------------------------------------------------------------
// User-input callbacks
//
// processNormalKeys: ESC, q, and Q cause program to exit
// releaseNormalKeys: r, R, d, D, 
// pressSpecialKey: Up arrow = forward motion, down arrow = backwards
// releaseSpecialKey: Set incremental motion to zero
//----------------------------------------------------------------------
void processNormalKeys(unsigned char key, int xx, int yy)
{
    switch (key)
    {
        case ESC : exit(0); break;
        case 'q' : exit(0); break;
        case 'Q' : exit(0); break;
        case 'r' : rotateBoard = true; break; // rotate the camera by 10 degrees (0.174533 radians)
        case 'R' : rotateBoard = true; break;
        case 'd' : deltaMove = -0.25; break; // move the position of the camera by 0.25
        case 'D' : deltaMove = -0.25; break;
        case 'u' : deltaMove = 0.25; break;
        case 'U' : deltaMove = 0.25; break;
        case '0' : light0Enable = light0Enable ? false : true ; break; // enable and disable light0 lighting
        case '1' : light1Enable = light1Enable ? false : true ; break; // enable and disable light1 lighting
        case 'k' : moveKnight = true; break;
        case 'K' : moveKnight = true; break;
        case 'p' : movePawn = true; break;
        case 'P' : movePawn = true; break;
    }
}

void releaseNormalKeys(unsigned char key, int xx, int yy)
{
    switch (key)
    {
        case 'd' : deltaMove = 0.0; break;
        case 'D' : deltaMove = 0.0; break;
        case 'u' : deltaMove = 0.0; break;
        case 'U' : deltaMove = 0.0; break;
    }
}

//----------------------------------------------------------------------
// Draw chess pieces (starting at the origin)
// White pieces drawn first followed by black pieces
// ----------------------------------------------------------------------
void drawChessPieces()
{
    int i;
    // ----------- Draw white chess pieces ----------- 
    glColor3f(140.0/255, 140.0/255, 135.0/255); // set drawing color to white

    // Draw white pawn pieces
    for (i = 0; i < 8; ++i) {
        glPushMatrix();
            glTranslatef(i + 0.5, whitePawnY[i] + 0.5, height / 2);
            glScalef(width, depth, height);
            glutSolidSphere(height / 2, 20, 20);
        glPopMatrix();
        updateSpotInfo(i, whitePawnY[i], 0); // set flag to indicate position is occupied
    }

    // Draw white rooks
    for (i = 0; i < 8; i += 7) {
        glPushMatrix();
            glTranslatef(i + 0.5, 0.5, height / 2);
            glScalef(width, depth, height);
            glutSolidCube(height);
        glPopMatrix();
        updateSpotInfo(i, 0, 0); // set flag to indicate position is occupied
    }

    // Draw white bishops
    for (i = 2; i < 6; i += 3) {
        glPushMatrix();
            glTranslatef(i + 0.5, 0.5, 0);
            glScalef(width, depth, height);
            glutSolidCone(height / 2, height, 20, 20);
        glPopMatrix();
        updateSpotInfo(i, 0, 0); // set flag to indicate position is occupied
    }

    // Draw white knight
    for (i = 0; i < 2; ++i) {
        glPushMatrix();
            glTranslatef(knightsX[i] + 0.5, knightsY[i] + 0.5, 0.45);
            glScalef(width, depth * 0.8, height * 1.2);
            glRotatef(90, 1.0, 0.0, 0.0);
            glRotatef(90, 0.0, 1.0, 0.0);
            glutSolidTeapot(0.5);
        glPopMatrix();
        updateSpotInfo(knightsX[i], knightsY[i], 0); // set flag to indicate position is occupied
    }

    // Draw white queen
    glPushMatrix();
        glTranslatef(3.5, 0.5, height / 2.6);
        glScalef(width / 2, depth / 2, height / 1.45);
        glRotatef(30, 0.0, 1.0, 0.0);
        glutSolidTetrahedron();
    glPopMatrix();
    updateSpotInfo(3, 0, 0); // set flag to indicate position is occupied

    // Draw white king
    glPushMatrix();
        glTranslatef(4.5, 0.5, height / 2);
        glScalef(width / 2, depth / 2, height / 2);
        glutSolidOctahedron();
    glPopMatrix();
    updateSpotInfo(4, 0, 0); // set flag to indicate position is occupied


    // ----------- Draw black chess pieces ----------- 
    glColor3f(150.0/255, 75.0/255, 0.0); // set drawing color to black

    // Draw black pawn pieces
    for (i = 0; i < 8; ++i) {
        glPushMatrix();
            glTranslatef(i + 0.5, blackPawnY[i] + 0.5, height / 2);
            glScalef(width, depth, height);
            glutSolidSphere(height / 2, 20, 20);
        glPopMatrix();
        updateSpotInfo(i, blackPawnY[i], 0); // set flag to indicate position is occupied
    }

    // Draw black rooks
    for (i = 0; i < 8; i += 7) {
        glPushMatrix();
            glTranslatef(i + 0.5, 7.5, height / 2);
            glScalef(width, depth, height);
            glutSolidCube(height);
        glPopMatrix();
        updateSpotInfo(i, 7, 0); // set flag to indicate position is occupied
    }

    // Draw black bishops
    for (i = 2; i < 6; i += 3) {
        glPushMatrix();
            glTranslatef(i + 0.5, 7.5, 0);
            glScalef(width, depth, height);
            glutSolidCone(height / 2, height, 20, 20);
        glPopMatrix();
        updateSpotInfo(i, 7, 0); // set flag to indicate position is occupied
    }
    
    // Draw black knight
    for (i = 2; i < 4; ++i) {
        glPushMatrix();
            glTranslatef(knightsX[i] + 0.5, knightsY[i] + 0.5, 0.45);
            glScalef(width, depth * 0.8, height * 1.2);
            glRotatef(90, 1.0, 0.0, 0.0);
            glRotatef(90, 0.0, 1.0, 0.0);
            glutSolidTeapot(0.5);
        glPopMatrix();
        updateSpotInfo(knightsX[i], knightsY[i], 0); // set flag to indicate position is occupied
    }

    // Draw black queen
    glPushMatrix();
        glTranslatef(3.5, 7.5, height / 2.6);
        glScalef(width / 2, depth / 2, height / 1.45);
        glRotatef(30, 0.0, 1.0, 0.0);
        glutSolidTetrahedron();
    glPopMatrix();
    updateSpotInfo(3, 7, 0); // set flag to indicate position is occupied

    // Draw black king
    glPushMatrix();
        glTranslatef(4.5, 7.5, height / 2);
        glScalef(width / 2, depth / 2, height / 2);
        glutSolidOctahedron();
    glPopMatrix();
    updateSpotInfo(4, 7, 0); // set flag to indicate position is occupied
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
        0.0, 0.0, 1.0);

    // translate the camera direction before rotating, then translate back
    glTranslatef(lx, ly, lz);
    glRotatef(angle, 0.0, 0.0, 1.0); // rotate chessboard
    glTranslatef(-lx, -ly, -lz);

    // draw chess board
    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            glNormal3f(0.0f, 0.0f, 1.0f);
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

    // printFreeSpot(); // print space occupancy map (0 means occupied)
    
    glutSwapBuffers(); // Make it all visible
}

//----------------------------------------------------------------------
// Main program  - standard GLUT initializations and callbacks
//----------------------------------------------------------------------
int main(int argc, char **argv)
{
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
    glutKeyboardUpFunc(releaseNormalKeys);

    // enter GLUT event processing cycle
    glutMainLoop();

    return 0; // this is just to keep the compiler happy
}