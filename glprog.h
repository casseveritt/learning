#pragma once

#include <GLES3/gl32.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

GLuint createProgram(const char *vertexShader, const char *fragmentShader);