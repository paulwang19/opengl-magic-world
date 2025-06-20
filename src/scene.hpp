#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/glew.h>
#include <GL/freeglut.h>

// Global variables defined in main.cpp
extern GLfloat angleX, angleY, angleZ;
extern GLuint woodTexture;

// Functions
void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

#endif // SCENE_HPP
