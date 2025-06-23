#include "obj_loader.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <GL/freeglut.h>
#include <iostream>

// Load
void LoadObjFile(const char* filename,
std::vector<Vertex>& vertices,
    std::vector<TexCoord>& texCoords,
    std::vector<Normal>& normals,
    std::vector<Face>& faces) {

    vertices.clear();
    texCoords.clear();
    normals.clear();
    faces.clear();

    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        }
        else if (type == "vt") {
            TexCoord t;
            iss >> t.u >> t.v;
            texCoords.push_back(t);
        }
        else if (type == "vn") {
            Normal n;
            iss >> n.nx >> n.ny >> n.nz;
            normals.push_back(n);
        }
        else if (type == "f") {
            Face face;
            for (int i = 0; i < 3; ++i) {
                std::string token;
                iss >> token;
                std::replace(token.begin(), token.end(), '/', ' ');
                std::istringstream tokenStream(token);
                tokenStream >> face.indices[i].vIdx >> face.indices[i].tIdx >> face.indices[i].nIdx;

                // OBJ index starts at 1
                face.indices[i].vIdx--;
                face.indices[i].tIdx--;
                face.indices[i].nIdx--;
            }
            faces.push_back(face);
        }
    }

    file.close();
 }

void DrawObjModel(const std::vector<Vertex>& vertices,
    const std::vector<TexCoord>& texCoords,
    const std::vector<Normal>& normals,
    const std::vector<Face>& faces) {
    glBegin(GL_TRIANGLES);
    for (const auto& face : faces) {
        for (int i = 0; i < 3; ++i) {
            const FaceIndex& idx = face.indices[i];
            if (!normals.empty())
                glNormal3f(normals[idx.nIdx].nx, normals[idx.nIdx].ny, normals[idx.nIdx].nz);
            if (!texCoords.empty())
                glTexCoord2f(texCoords[idx.tIdx].u, texCoords[idx.tIdx].v);
            glVertex3f(vertices[idx.vIdx].x, vertices[idx.vIdx].y, vertices[idx.vIdx].z);
        }
    }
    glEnd();
}

void loadOBJ(const char* filename, Model& model) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "無法開啟檔案：" << filename << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            Vec3s v;
            iss >> v.x >> v.y >> v.z;
            model.verticess.push_back(v);
        }
        else if (type == "vn") {
            Vec3s n;
            iss >> n.x >> n.y >> n.z;
            model.normalss.push_back(n);
        }
        else if (type == "f") {
            std::vector<int> vIndices;
            std::vector<int> tIndices;
            std::vector<int> nIndices;
            std::string token;

            while (iss >> token) {
                int vIdx = 0, tIdx = 0, nIdx = 0;
                sscanf_s(token.c_str(), "%d/%d/%d", &vIdx, &tIdx, &nIdx);
                vIndices.push_back(vIdx - 1);
                tIndices.push_back(tIdx - 1);
                nIndices.push_back(nIdx - 1);
            }

            model.facess.push_back(vIndices);
            model.faceTexIndices.push_back(tIndices);
            model.faceNormalIndices.push_back(nIndices);
        }
        else if (type == "vt") {
            Vec2s t;
            iss >> t.u >> t.v;
            model.texcoordss.push_back(t);
        }

    }

    std::cout << "讀取完成：" << filename << "："
        << model.verticess.size() << " 個頂點, "
        << model.facess.size() << " 個面" << std::endl;
}


