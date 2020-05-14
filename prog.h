#pragma once
#include "learning.h"

class Prog {
public:
  GLuint p;
  union Ui {
    GLuint u;
    GLint i;
  };
  Ui pos, col, norm, proj, view, model, lightPos, lightCol, matCol, camPos;

  void set(GLuint program);
};