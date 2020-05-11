#include "prog.h"

void Prog::set(GLuint program) {
  p = program;
  pos.i = glGetAttribLocation(program, "pos");
  col.i = glGetAttribLocation(program, "col");
  norm.i = glGetAttribLocation(program, "norm");
  proj.i = glGetUniformLocation(program, "proj");
  view.i = glGetUniformLocation(program, "view");
  model.i = glGetUniformLocation(program, "model");
  lightPos.i = glGetUniformLocation(program, "lightPos");
}