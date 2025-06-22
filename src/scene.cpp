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
    woodTexture = loadTGATexture("assets/textures/wood.tga");
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

    // Load Tower
    glColor3f(1.0f, 1.0f, 1.0f); // 確保模型亮一點（避免全黑）
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glPushMatrix();
    glTranslatef(0.0f, -1.0f, -5.0f);
    DrawObjModel(tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);

    // Draw the cube
    glPushMatrix();
    glTranslatef(-1.5, 0.0, 0.0); // Position the cube
    glColor3f(1.0, 0.0, 0.0); // Red color
    glutSolidCube(2.0);
    glPopMatrix();

    // Draw the sphere
    glPushMatrix();
    glTranslatef(1.5, 0.0, 0.0); // Position the sphere
    glColor3f(1.0, 1.0, 1.0); // Use white color for texturing
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 1.0, 50, 50);
    gluDeleteQuadric(quad);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix();

    // Swap the buffers
    glutSwapBuffers();
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
