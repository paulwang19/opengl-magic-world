#ifndef OBJ_LOADER_HPP
#define OBJ_LOADER_HPP

#include <vector>
#include <GL/freeglut.h>

struct Vertex {
    float x, y, z;
};

struct TexCoord {
    float u, v;
};

struct Normal {
    float nx, ny, nz;
};

struct FaceIndex {
    int vIdx, tIdx, nIdx;
};

struct Face {
    FaceIndex indices[3];
};


struct Vec3s {
	float x, y, z;
};

struct Vec2s {
	float u, v;
};

struct Model {
	std::vector<Vec3s> verticess;
	std::vector<Vec3s> normalss;
	std::vector<Vec2s> texcoordss;
	std::vector<std::vector<int>> facess;             // v index
	std::vector<std::vector<int>> faceTexIndices;     // vt index
	std::vector<std::vector<int>> faceNormalIndices;

};


void LoadObjFile(const char* filename, std::vector<Vertex>& vertices, std::vector<TexCoord>& texCoords, std::vector<Normal>& normals, std::vector<Face>& faces);
void DrawObjModel(const std::vector<Vertex>& vertices, const std::vector<TexCoord>& texCoords, const std::vector<Normal>& normals, const std::vector<Face>& faces);
void loadOBJ(const char* filename, Model& model);

#endif // OBJ_LOADER_HPP