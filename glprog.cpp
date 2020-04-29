#include "glprog.h"

#include "stb.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>

static char *getFileContents(const char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == nullptr) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char *data = new char[size + 1];
  size_t numRead = fread(data, 1, size_t(size), fp);
  if (size_t(size) != numRead) {
    printf("Failed to read all %s\n", filename);
    exit(20);
  }
  data[size] = 0;
  fclose(fp);
  return data;
}

GLuint createProgram(const char *vertexShaderFilename,
                     const char *fragmentShaderFilename) {
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char *vertex_shader_src = getFileContents(vertexShaderFilename);
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader);
  GLint stat = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Vertex comple failed\n");
  }
  delete[] vertex_shader_src;

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  char *fragment_shader_src = getFileContents(fragmentShaderFilename);
  glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Fragment comple failed\n");
  }
  delete[] fragment_shader_src;

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  return program;
}