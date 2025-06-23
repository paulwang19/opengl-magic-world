#include "scene.hpp"
#include "obj_loader.hpp"
#include "tga_loader.hpp"
#include <stdlib.h> // for exit()
#include <iostream>
#include <cmath>
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

TrebleClefObject trebleClef = TrebleClefObject();

// TrebleClef
TrebleClefObject::TrebleClefObject() : angle(0.0f), verticalAngle(0.0f), radius(7.0f), Amplitude(1.0f), _x(0.0f), _y(0.0f), _z(0.0f) {
    const char* file = "assets/models/TrebleClef01.obj";
    LoadObjFile(file, TrebleClefObject::_vertices, TrebleClefObject::_texCoords, TrebleClefObject::_normals, TrebleClefObject::_faces);
}

void TrebleClefObject::Update() {
    angle += 0.05f;
    verticalAngle += 0.08f;
    if ( angle > 2 * 3.1415 ) angle -= 2 * 3.1415;
    if ( verticalAngle > 2 * 3.1415 ) verticalAngle -= 2 * 3.1415;
}

void TrebleClefObject::DrawTrebleClef(bool forShadow) {
    // --- 根據 forShadow 參數決定是否啟用紋理與顏色 ---
    // 注意：每一幀都從硬碟讀取紋理是非常低效的。
    // 理想情況下，_texture 應該在 init() 或物件建構子中只載入一次。
    // 但此處我們僅根據要求修改。
    if (!forShadow) {
        _texture = loadTGATexture("assets/textures/TrebleClef.tga"); 
        glEnable(GL_TEXTURE_2D); 
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBindTexture(GL_TEXTURE_2D, _texture);
    }
    
    glPushMatrix();
    
    // 計算位置
    _x = radius * cos(angle);
    _z = radius * sin(angle);
    _y = Amplitude * sin(verticalAngle);
    glTranslatef(_x, _y, _z);
    
    glScalef(0.001f, 0.001f, 0.001f);

    // 繪製幾何體
    DrawObjModel(TrebleClefObject::_vertices, TrebleClefObject::_texCoords, TrebleClefObject::_normals, TrebleClefObject::_faces); 
    
    // --- 如果是繪製實體，結束後取消綁定 ---
    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    // 注意：glDisable(GL_TEXTURE_2D) 應該在 if 區塊外，或者由呼叫者管理
    // 但為保持與原始邏輯一致，我們讓它只在非陰影模式下解綁
    // 如果之前啟用了紋理，現在就關閉它
    if (!forShadow) {
        glDisable(GL_TEXTURE_2D); 
    }

    glPopMatrix();
}

float planetTime = 0.0f;
GLfloat angleX, angleY, angleZ;
GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 10.0f;
GLfloat lookAtX = 0.0f, lookAtY = 0.0f, lookAtZ = 0.0f;
GLfloat viewAngleY = 0.0f;
GLuint planetTexture, towerTexture, groundTexture, swordTexture, marbleTexture, woodTexture;
bool isPaused = false;
GLfloat moonAngle = 0.0f;
GLfloat swordSwayAngle = 0.0f;
GLfloat lightPos[] = {10.0f, 20.0f, 10.0f, 1.0f}; // Light position for shadows


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

    // Add a new light for shadows
    glEnable(GL_LIGHT1);
    GLfloat light1_position[] = { lightPos[0], lightPos[1], lightPos[2], lightPos[3] };
    GLfloat light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

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

void drawPlanet(bool forShadow = false) {
    glPushMatrix();
    
    // 計算行星的動態位置
    float yWave = cos(planetTime) * 1.0f;
    glTranslatef(-5.0f, 2.5f + yWave, 0.0f);
    glRotatef(planetAngleY, 0.0f, 1.0f, 0.0f);
    glScalef(0.5f, 0.5f, 0.5f);

    GLUquadric* quad = gluNewQuadric();
    
    // --- 根據 forShadow 參數決定是否啟用紋理與顏色 ---
    if (!forShadow) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, planetTexture);
        gluQuadricTexture(quad, GL_TRUE); // 啟用紋理座標生成
    }

    // 繪製球體 (無論是否為陰影都需要幾何體)
    gluSphere(quad, 1.0, 50, 50);
    
    // --- 如果是繪製實體，結束後取消綁定 ---
    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }

    gluDeleteQuadric(quad);
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

void drawStar(bool forShadow = false) {
    // --- 根據 forShadow 參數決定是否啟用材質、紋理與顏色 ---
    if (!forShadow) {
        // 讓材質發光
        GLfloat mat_emission[] = { 0.7, 0.7, 0.7, 1.0 };
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
    }

    GLUquadric* quad = gluNewQuadric();

    if (!forShadow) {
        gluQuadricTexture(quad, GL_TRUE);
    }

    // 繪製主體 (海膽的身體)
    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, marbleTexture);
    }
    gluSphere(quad, 0.8, 30, 30);

    // 繪製尖刺 (幾何形狀)
    const int num_layers = 7;
    const float spike_height = 0.7f;
    const float spike_base_radius = 0.04f;
    const float body_radius = 0.8f;
    
    // 在此處我們不為尖刺單獨綁定紋理，讓它繼承身體的狀態
    // 或在非陰影模式下，可以為它指定不同紋理
    // if (!forShadow) { glBindTexture(GL_TEXTURE_2D, woodTexture); }

    for (int i = 1; i < num_layers; ++i) {
        float lat = M_PI * i / num_layers;
        int num_spikes_on_layer = (int)(2 * M_PI * body_radius * sin(lat) / (4 * spike_base_radius));
        if (num_spikes_on_layer < 4) num_spikes_on_layer = 4;

        for (int j = 0; j < num_spikes_on_layer; ++j) {
            float lon = 2 * M_PI * j / num_spikes_on_layer;
            float x = body_radius * sin(lat) * cos(lon);
            float y = body_radius * sin(lat) * sin(lon);
            float z = body_radius * cos(lat);

            glPushMatrix();
            glTranslatef(x, y, z);
            float angle = acos(z / body_radius) * 180.0f / M_PI;
            glRotatef(angle, -y, x, 0.0f);
            gluCylinder(quad, spike_base_radius, 0, spike_height, 8, 1);
            glPopMatrix();
        }
    }

    // 繪製頂部和底部的尖刺
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

    // --- 如果是繪製實體，結束後重設狀態 ---
    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);

        // 重設發光材質
        GLfloat no_mat_emission[] = { 0.0, 0.0, 0.0, 1.0 };
        glMaterialfv(GL_FRONT, GL_EMISSION, no_mat_emission);
    }
}

void drawSwordAndSatellite(bool forShadow = false) {
    glPushMatrix();

    // 移動到劍和衛星系統的中心
    glTranslatef(0.0f, 3.0f, -5.0f);

    // --- 繪製劍 ---
    glPushMatrix();
    glScalef(0.1f, 0.1f, 0.1f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(swordSwayAngle, 0.0f, 1.0f, 0.0f);

    if (!forShadow) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, swordTexture);
    }
    
    DrawObjModel(sword_vertex_data, sword_texcoords_data, sword_normals_data, sword_face_data);
    
    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    glPopMatrix();

    // --- 繪製衛星 (海膽星) ---
    glPushMatrix();
    
    // 根據克卜勒定律計算位置
    float a = satelliteSemiMajorAxis;
    float e = satelliteEccentricity;
    float theta = satelliteTrueAnomaly;
    float r = a * (1 - e * e) / (1 + e * cos(theta));
    float satelliteX = r * cos(theta);
    float satelliteZ = r * sin(theta);
    glTranslatef(satelliteX, 0.0f, satelliteZ);
    glRotatef(planetTime * 50, 0.0, 0.0, 1.0);
    glScalef(0.5f, 0.5f, 0.5f);

    // 呼叫 drawStar 並傳遞 forShadow 參數
    drawStar(forShadow);

    // --- 繪製海膽星自己的衛星 (小木球) ---
    glPushMatrix();
    glRotatef(moonAngle, 0.0f, 1.0f, 0.0f);
    glTranslatef(3.0f, 0.0f, 0.0f);
    glScalef(0.4f, 0.4f, 0.4f);
    
    GLUquadric* quad = gluNewQuadric();
    if (!forShadow) {
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        gluQuadricTexture(quad, GL_TRUE);
    }

    gluSphere(quad, 1.0, 20, 20);

    if (!forShadow) {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
    gluDeleteQuadric(quad);
    glPopMatrix(); // 彈出小木球的變換

    glPopMatrix(); // 彈出海膽星的變換

    glPopMatrix(); // 彈出整個劍與衛星系統的變換
}


void drawModel(const Model& model, GLuint textureID, bool forShadow = false) {
    // --- 根據 forShadow 參數決定是否啟用紋理 ---
    if (!forShadow) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

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

            // 紋理座標總是會被設定，但如果 GL_TEXTURE_2D 是關閉的，它會被忽略
            if (tIdx >= 0 && tIdx < model.texcoordss.size()) {
                Vec2s uv = model.texcoordss[tIdx];
                glTexCoord2f(uv.u, 1.0f - uv.v);
            } else {
                glTexCoord2f(0.0f, 0.0f);
            }

            Vec3s v = model.verticess[vIdx];
            glVertex3f(v.x, v.y, v.z);
        }
    }
    glEnd();
    
    // --- 如果是繪製實體，結束後取消綁定 ---
    if (!forShadow) {
        glDisable(GL_TEXTURE_2D); // 解綁定(glBindTexture with 0)是好習慣，但此處直接關閉更符合邏輯
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}




void drawTower(bool forShadow = false) { // 新增 bool 參數，預設為 false
    if (!forShadow) {
        // 只有在不是繪製陰影時，才設定顏色和紋理
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, towerTexture);
    }

    glPushMatrix();
    glTranslatef(-5.0f, -2.0f, -5.0f);
    glScalef(0.7f, 0.7f, 0.7f);
    DrawObjModel(tower_vertex_data, tower_texcoords_data, tower_normals_data, tower_face_data);
    glPopMatrix();

    if (!forShadow) {
        // 繪製實體後，解綁紋理
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D);
    }
}

void drawDragon(bool forShadow = false) {
    // 繪製身體
    glPushMatrix();
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    // 將 forShadow 參數傳遞給 drawModel
    drawModel(bodyModel, dragonTexture, forShadow);
    glPopMatrix();

    // 繪製左翼
    glPushMatrix();
    glTranslatef(leftWingPivotX + dragonPosX, leftWingPivotY + dragonPosY, leftWingPivotZ + dragonPosZ);
    glRotatef(wingAngle, 0, 0, 1);
    glTranslatef(-leftWingPivotX - dragonPosX, -leftWingPivotY - dragonPosY, -leftWingPivotZ - dragonPosZ);
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    // 將 forShadow 參數傳遞給 drawModel
    drawModel(leftWingModel, dragonTexture, forShadow);
    glPopMatrix();

    // 繪製右翼
    glPushMatrix();
    glTranslatef(rightWingPivotX + dragonPosX, rightWingPivotY + dragonPosY, rightWingPivotZ + dragonPosZ);
    glRotatef(-wingAngle, 0, 0, 1);
    glTranslatef(-rightWingPivotX - dragonPosX, -rightWingPivotY - dragonPosY, -rightWingPivotZ - dragonPosZ);
    glTranslatef(dragonPosX, dragonPosY, dragonPosZ);
    glScalef(0.05f, 0.05f, 0.05f);
    // 將 forShadow 參數傳遞給 drawModel
    drawModel(rightWingModel, dragonTexture, forShadow);
    glPopMatrix();
}

void drawGrass() {
    glPushMatrix();
    glTranslatef(0.0f, -2.0f, 0.0f);
    glScalef(50.0f, 0.01f, 50.0f);
    drawModel(grassModel, grassTexture);
    glPopMatrix();
}

void drawTree(float x, float z, bool forShadow = false) {
    // 繪製樹根
    glPushMatrix();
    glTranslatef(x, -2.0f, z);
    glScalef(0.05f, 0.05f, 0.05f);
    // 將 forShadow 參數傳遞給 drawModel
    drawModel(rootModel, rootTexture, forShadow);
    glPopMatrix();

    // 繪製樹葉
    glPushMatrix();
    glTranslatef(x, -2.0f, z);
    glScalef(0.05f, 0.05f, 0.05f);
    // 將 forShadow 參數傳遞給 drawModel
    drawModel(leafModel, leafTexture, forShadow);
    glPopMatrix();
}

// Function to build the shadow projection matrix
void buildShadowMatrix(GLfloat matrix[16], const GLfloat light_pos[4], const GLfloat plane[4]) {
    GLfloat dot = plane[0] * light_pos[0] + plane[1] * light_pos[1] + plane[2] * light_pos[2] + plane[3] * light_pos[3];
    matrix[0] = dot - light_pos[0] * plane[0];
    matrix[1] = 0.0f - light_pos[1] * plane[0];
    matrix[2] = 0.0f - light_pos[2] * plane[0];
    matrix[3] = 0.0f - light_pos[3] * plane[0];
    matrix[4] = 0.0f - light_pos[0] * plane[1];
    matrix[5] = dot - light_pos[1] * plane[1];
    matrix[6] = 0.0f - light_pos[2] * plane[1];
    matrix[7] = 0.0f - light_pos[3] * plane[1];
    matrix[8] = 0.0f - light_pos[0] * plane[2];
    matrix[9] = 0.0f - light_pos[1] * plane[2];
    matrix[10] = dot - light_pos[2] * plane[2];
    matrix[11] = 0.0f - light_pos[3] * plane[2];
    matrix[12] = 0.0f - light_pos[0] * plane[3];
    matrix[13] = 0.0f - light_pos[1] * plane[3];
    matrix[14] = 0.0f - light_pos[2] * plane[3];
    matrix[15] = dot - light_pos[3] * plane[3];
}



// Display callback function
void display() {
    glClearColor(0.0f, 0.6f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(cameraX, cameraY, cameraZ,
              lookAtX, lookAtY, lookAtZ,
              0.0, 1.0, 0.0);

    glRotatef(angleX, 1.0, 0.0, 0.0);
    glRotatef(angleY, 0.0, 1.0, 0.0);
    glRotatef(angleZ, 0.0, 0.0, 1.0);

    // 1. 先繪製接收陰影的地面 (此函數不需要修改，因為它不投射陰影)
    drawGrass();

    // 2. 繪製陰影
    glPushMatrix();
    GLfloat shadow_matrix[16];
    GLfloat plane[] = {0.0f, 1.0f, 0.0f, 2.0f}; // y = -2.0f 的平面
    buildShadowMatrix(shadow_matrix, lightPos, plane);

    glDisable(GL_LIGHTING);   // 關閉光照
    glDisable(GL_TEXTURE_2D); // 關閉紋理 (雖然子函數會處理，但在這裡設置是好習慣)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // 設定陰影顏色為半透明黑色

    glDisable(GL_DEPTH_TEST); // 避免陰影和地面 Z-fighting

    glMultMatrixf(shadow_matrix);

    // 呼叫繪製函數，並傳入 true，表示只繪製陰影
    drawTower(true);
    drawPlanet(true);
    drawSwordAndSatellite(true);
    drawDragon(true);
    drawTree(-2.6f, 4.0f, true);
    drawTree(-1.8f, 3.4f, true);
    drawTree(-0.9f, 4.5f, true);
    float countree = 0.0f;
    for (int i = 1; i < 4; i++) {
        for (int j = 1; j < 6; j++) {
            drawTree(1.03f * j - countree, 0.9f*i - 2.3f, true);
        }
        countree += 0.4f;
    }
    trebleClef.DrawTrebleClef(true);

    glPopMatrix();

    // 還原渲染狀態
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    // 此處不需啟用 GL_TEXTURE_2D，讓各個繪圖函式自行管理
    glDisable(GL_BLEND);

    // 3. 繪製實際的物體
    // 呼叫繪製函數，使用預設值 (false)，表示繪製帶有材質的實體
    trebleClef.DrawTrebleClef();
    drawPlanet();
    drawTower();
    drawTree(-2.6f, 4.0f);
    drawTree(-1.8f, 3.4f);
    drawTree(-0.9f, 4.5f);
    countree = 0.0f;
    for (int i = 1; i < 4; i++) {
        for (int j = 1; j < 6; j++) {
            drawTree(1.03f * j - countree, 0.9f*i - 2.3f);
        }
        countree += 0.4f;
    }
    drawSwordAndSatellite();
    drawDragon();

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
        trebleClef.Update();
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
