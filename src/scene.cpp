#include "scene.hpp"
#include "obj_loader.hpp"
#include "tga_loader.hpp"
#include <stdlib.h> // for exit()
#include <vector>

using namespace std;

const char* file1 = "assets/models/Magic_tower.obj";

vector<Vertex> tower_vertex_data;
vector<Face> tower_face_data;
vector<Normal> tower_normals_data;
vector<TexCoord> tower_texcoords_data;

GLfloat planetAngleX, planetAngleY, planetAngleZ = 0.0f;
float planetTime = 0.0f;

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
}

void drawPlanet() {
    glPushMatrix();
    float yWave = cos(planetTime) * 1.0f; // 1.0f 是擺動幅度
    glTranslatef(-5.0f, 2.5f + yWave, 0.0f); // y 加上 cos 波擺動
    glRotatef(planetAngleY, 0.0f, 1.0f, 0.0f); // 自轉
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

void drawTower() {
    glColor3f(1.0f, 1.0f, 1.0f); // 確保模型亮一點（避免全黑）
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
    gluLookAt(0.0, 0.0, 10.0,  // eye position
              0.0, 0.0, 0.0,   // look-at position
              0.0, 1.0, 0.0);  // up vector

    // Apply rotations
    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);

    // Draw Function
    drawPlanet();
    drawTower();

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
    switch (key) {
    case 'x':
    case 'X':
        angleX += 5.0;
        break;
    case 'y':
    case 'Y':
        angleY += 5.0;
        break;
    case 'z':
    case 'Z':
        angleZ += 5.0;
        break;
    case 27: // ESC key
        exit(0);
        break;
    }
    // Request a redraw
    glutPostRedisplay();
}
