#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/glew.h>
#include <GL/freeglut.h>

// Global variables defined in main.cpp
extern GLfloat angleX, angleY, angleZ;
extern GLfloat cameraX, cameraY, cameraZ;
extern GLfloat lookAtX, lookAtY, lookAtZ;
extern GLuint planetTexture;
extern GLuint towerTexture;

// Functions
void init();
void display();
void reshape(int w, int h);
void idle();
void keyboard(unsigned char key, int x, int y);
void specialKeyboard(int key, int x, int y);
void update(int value);
void reset();

#endif // SCENE_HPP
