#include "obj_loader.hpp"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <GL/freeglut.h>

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


