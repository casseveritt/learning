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
      matSpcCol, shiny, camPos;

  void set(GLuint program);
};