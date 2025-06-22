#include "scene.hpp"
#include "obj_loader.hpp"
#include "tga_loader.hpp"
#include <stdlib.h> // for exit()
#include <vector>
#include <cmath>

using namespace std;

const char* file1 = "assets/models/Magic_tower.obj";

vector<Vertex> tower_vertex_data;
vector<Face> tower_face_data;
vector<Normal> tower_normals_data;
vector<TexCoord> tower_texcoords_data;

GLfloat planetAngleX, planetAngleY, planetAngleZ = 0.0f;
float planetTime = 0.0f;
GLfloat angleX, angleY, angleZ;
GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 10.0f;
GLfloat lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
GLfloat viewAngleY = 0.0f;
GLuint planetTexture, towerTexture, groundTexture;

// Function to reset the scene
void reset() {
    angleX = angleY = angleZ = 0.0f;
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 10.0f;
    lookAtX = 0.0f; lookAtY = 0.0f; lookAtZ = 0.0f;
    viewAngleY = 0.0f;
}

// Function to initialize OpenGL settings
void init() {
    LoadObjFile(file1, tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    // Set clear color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    // Enable lighting
    glEnable(GL_LIGHTING);
    // Enable light source 0
    glEnable(GL_LIGHT0);
    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    planetTexture = loadTGATexture("assets/textures/planet.tga");
    towerTexture = loadTGATexture("assets/textures/tower.tga");
    groundTexture = loadTGATexture("assets/textures/chessboard.tga");
    reset();
}

void drawPlanet() {
    glPushMatrix();
    float yWave = cos(planetTime) * 1.0f; // 1.0f �O�\�ʴT��
    glTranslatef(-5.0f, 2.5f + yWave, 0.0f); // y �[�W cos �i�\��
    glRotatef(planetAngleY, 0.0f, 1.0f, 0.0f); // ����
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, planetTexture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    glScalef(0.5f, 0.5f, 0.5f);
    gluSphere(quad, 1.0, 50, 50);
    gluDeleteQuadric(quad);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();
}

void drawGround() {
    GLfloat mat_emission[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glPushMatrix();
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-100.0f, -2.0f, -100.0f);
        glTexCoord2f(0.0f, 50.0f); glVertex3f(-100.0f, -2.0f,  100.0f);
        glTexCoord2f(50.0f, 50.0f); glVertex3f( 100.0f, -2.0f,  100.0f);
        glTexCoord2f(50.0f, 0.0f); glVertex3f( 100.0f, -2.0f, -100.0f);
    glEnd();
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);

    GLfloat no_mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat_emission);
}

void drawTower() {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for texturing
    glBindTexture(GL_TEXTURE_2D, towerTexture);
    glPushMatrix();
    glTranslatef(-5.0f, -2.0f, -5.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    DrawObjModel(tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Display callback function
void display() {
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();

    // Set the camera
    gluLookAt(cameraX, cameraY, cameraZ,  // eye position
              lookAtX, lookAtY, lookAtZ,   // look-at position
              0.0, 1.0, 0.0);  // up vector

    // Apply rotations
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);

    // Draw Function
    drawPlanet();
    drawTower();
    drawGround();

    // Swap the buffers
    glutSwapBuffers();
}

void update(int value) {
    planetAngleY += 2.0f;
    if (planetAngleY > 360)
        planetAngleY -= 360;

    planetTime += 0.05f;

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Reshape callback function
void reshape(int w, int h) {
    // Set the viewport to the entire window
    glViewport(0, 0, w, h);
    // Set the projection matrix
    glMatrixMode(GL_PROJECTION);
    // Reset the projection matrix
    glLoadIdentity();
    // Set the perspective
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
    // Switch back to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    float turnSpeed = 0.05f;
    bool viewChanged = false;

    switch (key) {
    case 'r':
    case 'R':
        reset();
        break;
    case 'q':
    case 'Q':
        viewAngleY -= turnSpeed;
        viewChanged = true;
        break;
    case 'w':
    case 'W':
        viewAngleY += turnSpeed;
        viewChanged = true;
        break;
    case ' ':
        cameraY += 0.5f;
        lookAtY += 0.5f;
        break;
    case 'z':
    case 'Z':
        cameraY -= 0.5f;
        lookAtY -= 0.5f;
        break;
    case 27: // ESC key
        exit(0);
        break;
    }

    if (viewChanged) {
        lookAtX = cameraX + sin(viewAngleY);
        lookAtZ = cameraZ - cos(viewAngleY);
    }
    // Request a redraw
    glutPostRedisplay();
}

void specialKeyboard(int key, int x, int y) {
    float moveSpeed = 0.5f;
    float dirX = sin(viewAngleY);
    float dirZ = -cos(viewAngleY);

    switch (key) {
        case GLUT_KEY_UP:
            cameraX += dirX * moveSpeed;
            cameraZ += dirZ * moveSpeed;
            break;
        case GLUT_KEY_DOWN:
            cameraX -= dirX * moveSpeed;
            cameraZ -= dirZ * moveSpeed;
            break;
        case GLUT_KEY_LEFT:
            cameraX += dirZ * moveSpeed;
            cameraZ -= dirX * moveSpeed;
            break;
        case GLUT_KEY_RIGHT:
            cameraX -= dirZ * moveSpeed;
            cameraZ += dirX * moveSpeed;
            break;
    }

    lookAtX = cameraX + dirX;
    lookAtZ = cameraZ + dirZ;

    glutPostRedisplay();
}
