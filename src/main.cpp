#include <GL/glew.h>
#include <GL/freeglut.h>
#include "scene.hpp"

// Rotation angles
GLfloat angleX = 0.0;
GLfloat angleY = 0.0;
GLfloat angleZ = 0.0;
GLuint woodTexture;

// Main function
int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Set the display mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Set the window size
    glutInitWindowSize(800, 600);
    // Create the window
    glutCreateWindow("Rotating Sphere and Cube");

    // Initialize OpenGL settings
    init();
    // test comment

    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Enter the GLUT event processing loop
    glutMainLoop();

    return 0;
}