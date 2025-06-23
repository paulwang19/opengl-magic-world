#include "scene.hpp"
#include "obj_loader.hpp"
#include "tga_loader.hpp"
#include <stdlib.h> // for exit()
#include <vector>
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;

const char* file1 = "assets/models/Magic_tower.obj";
const char* file2 = "assets/models/sword.obj";

vector<Vertex> tower_vertex_data;
vector<Face> tower_face_data;
vector<Normal> tower_normals_data;
vector<TexCoord> tower_texcoords_data;

vector<Vertex> sword_vertex_data;
vector<Face> sword_face_data;
vector<Normal> sword_normals_data;
vector<TexCoord> sword_texcoords_data;

GLfloat planetAngleX, planetAngleY, planetAngleZ = 0.0f;
float planetTime = 0.0f;
GLfloat angleX, angleY, angleZ;
GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 10.0f;
GLfloat lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
GLfloat viewAngleY = 0.0f;
GLuint planetTexture, towerTexture, groundTexture, swordTexture, marbleTexture, woodTexture;
bool isPaused = false;
GLfloat moonAngle = 0.0f;
GLfloat swordSwayAngle = 0.0f;


Model bodyModel;
Model leftWingModel;
Model rightWingModel;
Model grassModel;
Model rootModel;
Model leafModel;
GLuint dragonTexture, grassTexture, rootTexture, leafTexture;
float wingAngle = 0.0f;         // 翅膀目前角度
float wingSpeed = 2.0f;         // 擺動速度（radian/sec
float timeElapsed = 0.0f;       // 經過時間

float leftWingPivotX = 0.15f;
float leftWingPivotY = 0.9f;
float leftWingPivotZ = 0.0f;

float rightWingPivotX = -0.15f;
float rightWingPivotY = 0.9f;
float rightWingPivotZ = 0.0f;

float dragonPosX = 1.5f;
float dragonPosY = -2.0f;
float dragonPosZ = 3.0f;


// Kepler's law parameters for satellite
const float satelliteSemiMajorAxis = -3.0f;
const float satelliteEccentricity = 0.5f;
float satelliteTrueAnomaly = 0.0f;

// Function to reset the scene
void reset() {
    angleX = angleY = angleZ = 0.0f;
    cameraX = 0.0f; cameraY = 0.0f; cameraZ = 10.0f;
    lookAtX = 0.0f; lookAtY = 0.0f; lookAtZ = 0.0f;
    viewAngleY = 0.0f;
    satelliteTrueAnomaly = 0.0f;
    moonAngle = 0.0f;
    swordSwayAngle = 0.0f;
}

// Function to initialize OpenGL settings
void init() {
    LoadObjFile(file1, tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    LoadObjFile(file2, sword_vertex_data, sword_texcoords_data, sword_normals_data, sword_face_data);
    // Set clear color to black
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    // Enable lighting
    glEnable(GL_LIGHTING);
    // Enable light source 0
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    GLfloat light_position[] = { 0.0f, 5.0f, 10.0f, 1.0f };
    GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

    // Enable color material
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
    planetTexture = loadTGATexture("assets/textures/planet.tga");
    towerTexture = loadTGATexture("assets/textures/tower.tga");
    groundTexture = loadTGATexture("assets/textures/chessboard.tga");
    swordTexture = loadTGATexture("assets/textures/s.tga");
    marbleTexture = loadTGATexture("assets/textures/marble.tga");
    woodTexture = loadTGATexture("assets/textures/wood.tga");

    // 載入模型
    loadOBJ("assets/models/dragon_body.obj", bodyModel);
    loadOBJ("assets/models/left_wing.obj", leftWingModel);
    loadOBJ("assets/models/right_wing.obj", rightWingModel);
    loadTextureWithOpenCV("assets/textures/dragon_skin.jpg", dragonTexture);
    loadOBJ("assets/models/Grass.obj", grassModel);
    loadTextureWithOpenCV("assets/textures/grass.jpg", grassTexture);
    loadOBJ("assets/models/root.obj", rootModel);
    loadTextureWithOpenCV("assets/textures/root.jpg", rootTexture);
    loadOBJ("assets/models/leaf.obj", leafModel);
    loadTextureWithOpenCV("assets/textures/sakura.jpg", leafTexture);

    reset();
}

void drawPlanet() {
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
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
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawGround() {
    GLfloat mat_emission[] = { 1.0, 1.0, 1.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
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

    glDisable(GL_TEXTURE_2D);
    GLfloat no_mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat_emission);
}

void drawStar() {
    // Make the material bright
    GLfloat mat_emission[] = { 0.7, 0.7, 0.7, 1.0 }; // Bright white emission
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    glEnable(GL_TEXTURE_2D);

    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);

    // Draw the main body (sphere) of the sea urchin
    glBindTexture(GL_TEXTURE_2D, marbleTexture);
    glColor3f(1.0f, 1.0f, 1.0f);
    gluSphere(quad, 0.8, 30, 30);

    // Draw the spikes (cones)
    glBindTexture(GL_TEXTURE_2D, marbleTexture); // Using wood texture for spikes
    glColor3f(1.0f, 1.0f, 1.0f);

    const int num_layers = 7; // Number of layers of spikes
    const float spike_height = 0.7f;
    const float spike_base_radius = 0.04f;
    const float body_radius = 0.8f;

    // Draw spikes in layers
    for (int i = 1; i < num_layers; ++i) {
        float lat = M_PI * i / num_layers;
        // Calculate how many spikes to draw on this layer based on circumference
        int num_spikes_on_layer = (int)(2 * M_PI * body_radius * sin(lat) / (4 * spike_base_radius));
        if (num_spikes_on_layer < 4) num_spikes_on_layer = 4;

        for (int j = 0; j < num_spikes_on_layer; ++j) {
            float lon = 2 * M_PI * j / num_spikes_on_layer;

            // Convert spherical coordinates to Cartesian for spike position
            float x = body_radius * sin(lat) * cos(lon);
            float y = body_radius * sin(lat) * sin(lon);
            float z = body_radius * cos(lat);

            glPushMatrix();
            // Move to the spike's position on the sphere's surface
            glTranslatef(x, y, z);

            // Rotate the coordinate system to make the spike point outwards
            float angle = acos(z / body_radius) * 180.0f / M_PI;
            glRotatef(angle, -y, x, 0.0f);

            // Draw the spike as a cone
            gluCylinder(quad, spike_base_radius, 0, spike_height, 8, 1);
            glPopMatrix();
        }
    }

    // Add spikes at the poles to cover the top and bottom
    glPushMatrix();
    glTranslatef(0, 0, body_radius);
    gluCylinder(quad, spike_base_radius, 0, spike_height, 8, 1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, -body_radius);
    glRotatef(180, 1, 0, 0);
    gluCylinder(quad, spike_base_radius, 0, spike_height, 8, 1);
    glPopMatrix();

    gluDeleteQuadric(quad);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);

    // Reset emission
    GLfloat no_mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat_emission);
}

void drawSwordAndSatellite() {
    // Save the current matrix
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    // Translate to the sword's base position
    glTranslatef(0.0f, 3.0f, -5.0f);

    // --- Draw the Sword ---
    glPushMatrix();
    // Apply sword-specific transformations
    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

    // Apply sway animation
    glRotatef(swordSwayAngle, 0.0f, 1.0f, 0.0f);

    // Bind texture and draw model
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, swordTexture);
    DrawObjModel(sword_vertex_data, sword_texcoords_data, sword_normals_data, sword_face_data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix(); // Restore to the sword's base position

    // --- Draw the Satellite ---
    glPushMatrix();
    
    // Position based on Kepler's laws
    float a = satelliteSemiMajorAxis;
    float e = satelliteEccentricity;
    float theta = satelliteTrueAnomaly;
    float r = a * (1 - e * e) / (1 + e * cos(theta));

    // The sword is at the focus. The orbit is in the XZ plane.
    float satelliteX = r * cos(theta);
    float satelliteZ = r * sin(theta);
    glTranslatef(satelliteX, 0.0f, satelliteZ);

    // Rotate the star to make it spin
    glRotatef(planetTime * 50, 0.0, 0.0, 1.0);
    glScalef(0.5f, 0.5f, 0.5f);

    // Draw the star
    drawStar();

    // --- Draw the star's satellite ---
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glRotatef(moonAngle, 0.0f, 1.0f, 0.0f); // Orbit around the star's Y-axis
    glTranslatef(3.0f, 0.0f, 0.0f); // Orbit radius
    glScalef(0.4f, 0.4f, 0.4f); // Scale down the satellite
    glColor3f(1.0f, 1.0f, 1.0f);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    GLUquadric* quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluSphere(quad, 1.0, 20, 20);
    gluDeleteQuadric(quad);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPopMatrix(); // Restore to the sword's base position

    // Restore the original matrix
    glPopMatrix();
}


void drawModel(const Model& model, GLuint textureID) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    //glScalef(0.05f, 0.05f, 0.05f);
    glBegin(GL_TRIANGLES);
    for (size_t f = 0; f < model.facess.size(); ++f) {
        for (size_t i = 0; i < model.facess[f].size(); ++i) {
            int vIdx = model.facess[f][i];
            int tIdx = model.faceTexIndices[f][i];
            int nIdx = model.faceNormalIndices[f][i];

            if (nIdx >= 0 && nIdx < model.normalss.size()) {
                Vec3s n = model.normalss[nIdx];
                glNormal3f(n.x, n.y, n.z);
            }

            // 防止非法索引
            if (tIdx >= 0 && tIdx < model.texcoordss.size()) {
                Vec2s uv = model.texcoordss[tIdx];
                glTexCoord2f(uv.u, 1.0f - uv.v);
            }
            else {
                // 給預設 UV，避免失敗
                glTexCoord2f(0.0f, 0.0f);
            }

            Vec3s v = model.verticess[vIdx];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
}




void drawTower() {
    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white for texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, towerTexture);
    glPushMatrix();
    glTranslatef(-5.0f, -2.0f, -5.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    DrawObjModel(tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    glPopMatrix();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void drawDragon() {
    //glPushMatrix();
    glPushMatrix();
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    drawModel(bodyModel, dragonTexture);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(leftWingPivotX + dragonPosX, leftWingPivotY + dragonPosY, leftWingPivotZ + dragonPosZ); // 移到接點
    glRotatef(wingAngle, 0, 0, 1);
    glTranslatef(-leftWingPivotX - dragonPosX, -leftWingPivotY - dragonPosY, -leftWingPivotZ - dragonPosZ); // 移回
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    drawModel(leftWingModel, dragonTexture);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rightWingPivotX + dragonPosX, rightWingPivotY + dragonPosY, rightWingPivotZ + dragonPosZ); // 移到接點
    glRotatef(-wingAngle, 0, 0, 1);
    glTranslatef(-rightWingPivotX - dragonPosX, -rightWingPivotY - dragonPosY, -rightWingPivotZ - dragonPosZ); // 移回
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    drawModel(rightWingModel, dragonTexture);
    glPopMatrix();
    //glPopMatrix();
}

void drawGrass() {
    glPushMatrix();
    glTranslatef(0.0f, -2.0f, 0.0f);
    glScalef(50.0f, 0.01f, 50.0f);
    drawModel(grassModel, grassTexture);
    glPopMatrix();
}

void drawTree(float x, float z) {
    glPushMatrix();
    glTranslatef(x, -2.0f, z);
    glScalef(0.05f, 0.05f, 0.05f);
    drawModel(rootModel, rootTexture);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(x, -2.0f, z);
    glScalef(0.05f, 0.05f, 0.05f);
    drawModel(leafModel, leafTexture);
    glPopMatrix();
}



// Display callback function
void display() {
    glClearColor(0.0f, 0.6f, 0.9f, 1.0f);
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
    drawGrass();
    drawTree(-2.6f, 4.0f);
    drawTree(-1.8f, 3.4f);
    drawTree(-0.9f, 4.5f);
    float countree = 0.0f;
    for (int i = 1; i < 4; i++) {
        for (int j = 1; j < 6; j++) {
            drawTree(1.03f * j - countree, 0.9f*i - 2.3f);
        }
        countree += 0.4f;
    }
    //drawGround();
    drawSwordAndSatellite();
    
    drawDragon();
    
    // Swap the buffers
    glutSwapBuffers();
}

void update(int value) {
    static int lastTime = 0;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    if (lastTime == 0) {
        lastTime = currentTime;
    }

    float deltaTime = (currentTime - lastTime) / 1000.0f;

    if (!isPaused) {
        timeElapsed += deltaTime;
        wingAngle = sin(timeElapsed * wingSpeed) * 30.0f;

        planetAngleY += 2.0f;
        if (planetAngleY > 360)
            planetAngleY -= 360;

        planetTime += 0.05f;

        // Update satellite position based on Kepler's 2nd Law
        float a = satelliteSemiMajorAxis;
        float e = satelliteEccentricity;
        float r = a * (1 - e * e) / (1 + e * cos(satelliteTrueAnomaly));
        
        // d(theta)/dt is proportional to 1/r^2
        float angular_velocity_factor = 0.3f; // Adjust for speed
        float d_theta = angular_velocity_factor / (r * r);
        satelliteTrueAnomaly += d_theta;
        if (satelliteTrueAnomaly > 2 * M_PI) {
            satelliteTrueAnomaly -= 2 * M_PI;
        }

        moonAngle += 5.0f; // Controls the speed of the new satellite
        if (moonAngle > 360.0f) {
            moonAngle -= 360.0f;
        }
        
        // Sword sway animation
        swordSwayAngle = sin(planetTime * 2.0f) * 5.0f;
    }

    lastTime = currentTime;

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

void idle() {
    // Animation logic has been moved to the update() function for better performance.
}


// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    float turnSpeed = 0.05f;
    bool viewChanged = false;

    switch (key) {
    case 'p':
    case 'P':
        isPaused = !isPaused;
        break;
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
