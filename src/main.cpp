#include <GL/glew.h>
#include <GL/freeglut.h>
#include "scene.hpp"

// Main function
int main(int argc, char** argv) {
    // Initialize GLUT
    glutInit(&argc, argv);
    // Set the display mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Set the window size
    glutInitWindowSize(800, 600);
    // Create the window
    glutCreateWindow("Magic World");

    // Initialize OpenGL settings
    init();

    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutTimerFunc(16, update, 0);

    // Enter the GLUT event processing loop
    glutMainLoop();

    return 0;
}