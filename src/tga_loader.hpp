#ifndef TGA_LOADER_HPP
#define TGA_LOADER_HPP

#include <GL/glew.h>
#include <string>

GLuint loadTGATexture(const char *fileName);
bool loadTextureWithOpenCV(const std::string& path, GLuint& textureID);

#endif // TGA_LOADER_HPP
