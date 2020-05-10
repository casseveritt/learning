#include "glprog.h"

#include "stb.h"
#include "learning.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

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

char * concatenate(const char * a, const char * b) {
  int alen = strlen(a);
  int blen = strlen(b);
  char * c = new char[alen + blen];
  strcpy(c, a);
  strcat(c, b);
  return c;
}

GLuint createProgram(const char *vertexShaderFilename,
                     const char *fragmentShaderFilename) {
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char * vertex_shader_src = nullptr;
  {
    char *f = getFileContents(vertexShaderFilename);
    vertex_shader_src = concatenate(GLSL_VERSION, f);
    delete [] f;
  }
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader);
  GLint stat = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Vertex comple failed\n");
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetShaderInfoLog( vertex_shader, sizeof(infoLog), &len, infoLog );
    printf( "infoLog: %s\n", (char *)infoLog );
  }
  delete[] vertex_shader_src;

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  char *fragment_shader_src = nullptr;
  {
    char * f = getFileContents(fragmentShaderFilename);
    fragment_shader_src = concatenate(GLSL_VERSION, f);
    delete [] f;
  }
  glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Fragment comple failed\n");
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetShaderInfoLog( fragment_shader, sizeof(infoLog), &len, infoLog );
    printf( "infoLog: %s\n", (char *)infoLog );
  }
  delete[] fragment_shader_src;

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  return program;
}