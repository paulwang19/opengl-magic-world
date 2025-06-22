#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "obj_loader.hpp"
#include "tga_loader.hpp"
#include <vector>

// Global variables defined in main.cpp
extern GLfloat angleX, angleY, angleZ;
extern GLuint planetTexture;
extern GLuint towerTexture;

// Functions
void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void update(int value);

class TrebleClefObject {
public:
    std::vector<Vertex> _vertices;
    std::vector<TexCoord> _texCoords;
    std::vector<Normal> _normals;
    std::vector<Face> _faces;
    GLuint _texture;
    TrebleClefObject();
    void DrawTrebleClef();
    void Update();
};
#endif // SCENE_HPP
