#include "prog.h"

#include "learning.h"
#include "stb.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

char* Prog::getFileContents(const char *filename) {
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

char* Prog::concatenate(const char *a, const char *b) {
  int alen = strlen(a);
  int blen = strlen(b);
  char *c = new char[alen + blen + 1];
  strcpy(c, a);
  strcat(c, b);
  return c;
}

GLuint Prog::createProgram(const char *vertexShaderFilename,
                     const char *fragmentShaderFilename) {
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char *vertex_shader_src = nullptr;
  {
    char *f = getFileContents(vertexShaderFilename);
    vertex_shader_src = concatenate(GLSL_VERSION, f);
    delete[] f;
  }
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader);
  GLint stat = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Vertex comple failed\n");
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetShaderInfoLog(vertex_shader, sizeof(infoLog), &len, infoLog);
    printf("infoLog: %s\n", static_cast<const char *>(infoLog));
  }
  delete[] vertex_shader_src;

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  char *fragment_shader_src = nullptr;
  {
    char *f = getFileContents(fragmentShaderFilename);
    fragment_shader_src = concatenate(GLSL_VERSION, f);
    delete[] f;
  }
  glShaderSource(fragment_shader, 1, &fragment_shader_src, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Fragment comple failed: %s\n", fragmentShaderFilename);
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetShaderInfoLog(fragment_shader, sizeof(infoLog), &len, infoLog);
    printf("infoLog: %s\n", static_cast<const char *>(infoLog));
  }
  delete[] fragment_shader_src;

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Program link failed: %s\n", vertexShaderFilename);
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetProgramInfoLog(program, sizeof(infoLog), &len, infoLog);
    printf("infoLog: %s\n", static_cast<const char *>(infoLog));
  }
  return program;
}

void Prog::set(GLuint program) {
  p = program;
  pos.i = glGetAttribLocation(program, "pos");
  col.i = glGetAttribLocation(program, "col");
  texCoord.i = glGetAttribLocation(program, "texCoord");
  norm.i = glGetAttribLocation(program, "norm");
  proj.i = glGetUniformLocation(program, "proj");
  view.i = glGetUniformLocation(program, "view");
  model.i = glGetUniformLocation(program, "model");
  lightPos.i = glGetUniformLocation(program, "lightPos");
  lightCol.i = glGetUniformLocation(program, "lightCol");
  matDifCol.i = glGetUniformLocation(program, "matDifCol");
  matSpcCol.i = glGetUniformLocation(program, "matSpcCol");
  shiny.i = glGetUniformLocation(program, "shiny");
  camPos.i = glGetUniformLocation(program, "camPos");
  samp.i = glGetUniformLocation(program, "samp");
}