#include "tga_loader.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/freeglut.h>

// A simple TGA loader
GLuint loadTGATexture(const char *fileName)
{
    GLuint textureID;
    GLubyte TGAheader[12] = {0,0,2,0,0,0,0,0,0,0,0,0}; // Uncompressed TGA Header
    GLubyte TGAcompare[12];
    GLubyte header[6];
    GLuint bytesPerPixel;
    GLuint imageSize;
    GLuint temp;
    GLuint type = GL_RGBA;

    FILE *file;
    fopen_s(&file, fileName, "rb");

    if(file == NULL ||
       fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
       memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
       fread(header, 1, sizeof(header), file) != sizeof(header))
    {
        if (file != NULL)
            fclose(file);
        return 0;
    }

    GLuint width = header[1] * 256 + header[0];
    GLuint height = header[3] * 256 + header[2];

    if(width <= 0 || height <= 0 || (header[4] != 24 && header[4] != 32))
    {
        fclose(file);
        return 0;
    }

    bytesPerPixel = header[4] / 8;
    imageSize = width * height * bytesPerPixel;

    GLubyte *imageData = (GLubyte*)malloc(imageSize);

    if(imageData == NULL || fread(imageData, 1, imageSize, file) != imageSize)
    {
        if(imageData != NULL)
            free(imageData);
        fclose(file);
        return 0;
    }

    fclose(file);

    // bgr to rgb
    for(GLuint i = 0; i < (int)imageSize; i += bytesPerPixel)
    {
        temp = imageData[i];
        imageData[i] = imageData[i + 2];
        imageData[i + 2] = temp;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (bytesPerPixel == 3)
        type = GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, imageData);

    free(imageData);

    return textureID;
}
