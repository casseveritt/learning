#pragma once
#include "learning.h"

class Prog {
public:
  GLuint p;
  union Ui {
    GLuint u;
    GLint i;
  };
  Ui pos, col, texCoord, norm, proj, view, model, lightPos, lightCol, matDifCol,
      matSpcCol, shiny, camPos, samp;

  char *getFileContents(const char *filename);

  char *concatenate(const char *a, const char *b);

  GLuint createProgram(const char *vertexShaderFilename,
                       const char *fragmentShaderFilename);

  void set(GLuint program);
};