#ifndef TEXTURE_H_DEFINED
#define TEXTURE_H_DEFINED

#include <string>
#include <GL/glew.h>
#include <vector>
#include <iostream>

GLuint loadTexture2DFromFilePath(const std::string& path);

void setDefaultTexture2DParameters(GLuint texture);

unsigned int loadCubemap(std::vector<std::string> faces);

#endif
