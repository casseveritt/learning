#include "prog.h"

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include "learning.h"
#include "stb.h"

namespace {

char* getFileContents(const char* filename) {
  FILE* fp = fopen(filename, "r");
  if (fp == nullptr) {
    printf("Failed to open file: %s\n", filename);
    exit(1);
  }
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  char* data = new char[size + 1];
  size_t numRead = fread(data, 1, size_t(size), fp);
  if (size_t(size) != numRead) {
    printf("Failed to read all %s\n", filename);
    exit(20);
  }
  data[size] = 0;
  fclose(fp);
  return data;
}

char* concatenate(const char* a, const char* b) {
  int alen = strlen(a);
  int blen = strlen(b);
  char* c = new char[alen + blen + 1];
  strcpy(c, a);
  strcat(c, b);
  return c;
}

}  // namespace

void Prog::create(const char* baseShaderName) {
  const char* pr = "progs/";
  const char* fs = "-Fragment.fs";
  const char* vs = "-Vertex.vs";
  char* bsfs = new char[strlen(pr) + strlen(baseShaderName) + strlen(fs) + 1];
  char* bsvs = new char[strlen(pr) + strlen(baseShaderName) + strlen(vs) + 1];
  strcpy(bsfs, pr);
  strcpy(bsvs, pr);
  strcat(bsfs, baseShaderName);
  strcat(bsvs, baseShaderName);
  strcat(bsfs, fs);
  strcat(bsvs, vs);
  create(bsvs, bsfs);
  delete[] bsvs;
  delete[] bsfs;
}

void Prog::create(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  char* vertex_shader_src = nullptr;
  {
    char* f = getFileContents(vertexShaderFilename);
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
    printf("infoLog: %s\n", static_cast<const char*>(infoLog));
  }
  delete[] vertex_shader_src;

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  char* fragment_shader_src = nullptr;
  {
    char* f = getFileContents(fragmentShaderFilename);
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
    printf("infoLog: %s\n", static_cast<const char*>(infoLog));
  }
  delete[] fragment_shader_src;

  p = glCreateProgram();

  glAttachShader(p, vertex_shader);
  glAttachShader(p, fragment_shader);
  glLinkProgram(p);
  glGetProgramiv(p, GL_LINK_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Program link failed: %s\n", vertexShaderFilename);
    GLchar infoLog[4096];
    GLsizei len = 0;
    glGetProgramInfoLog(p, sizeof(infoLog), &len, infoLog);
    printf("infoLog: %s\n", static_cast<const char*>(infoLog));
  }

  glUseProgram(p);
  for (auto v : programVars) {
    switch (v.inputType) {
      case InAttrib:
        locs[v.varName].i = glGetAttribLocation(p, v.varName);
        break;
      case InUniform:
        locs[v.varName].i = glGetUniformLocation(p, v.varName);
        break;
      default:
        break;
    }
  }
}

Prog::Ui Prog::loc(const std::string& str) const {
  auto it = locs.find(str);
  if (it == locs.end()) {
    Ui ui;
    ui.i = -1;
    return ui;
  }
  return it->second;
}

void Prog::load(const Scene& scene) {
  glUniform3fv(loc("lightPos").i, 1, &scene.lightPos.x);
  glUniform3fv(loc("lightCol").i, 1, &scene.lightCol.x);
  glUniform3fv(loc("camPos").i, 1, &scene.camPos.x);
  glUniform1i(loc("samp").i, 0);
}