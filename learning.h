#pragma once

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GLSL_VERSION "#version 410\n"
#else
#include <GLES3/gl32.h>
#define GLFW_INCLUDE_NONE
#define GLSL_VERSION "#version 320 es\n"
#endif

#include <GLFW/glfw3.h>

inline void make_hints() {
#if defined(__APPLE__)
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif
}