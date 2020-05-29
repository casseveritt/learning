#pragma once
#include "learning.h"
#include "scene.h"

class Prog {
public:
  GLuint p;
  union Ui {
    GLuint u;
    GLint i;
  };
  Ui pos, col, texCoord, norm, proj, view, model, lightPos, lightCol, matDifCol,
      matSpcCol, shiny, camPos, samp;

  Prog() {}

  Prog(const char *baseShaderName) { create(baseShaderName); }

  Prog(const char *vertexShaderFilename, const char *fragmentShaderFilename) {
    create(vertexShaderFilename, fragmentShaderFilename);
  }

  void create(const char *vertexShaderFilename,
              const char *fragmentShaderFilename);
  void create(const char *baseShaderName);

  void load(const Scene &scene);
};