#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "gles3jni.h"
#include "scene.h"

enum ShaderType {
  StScalar,
  StVector,
  StMatrix,
  StSampler,
};

enum InputType { InAttrib, InUniform };

struct Var {
  const char* varName;
  InputType inputType;
  GLenum type;
  ShaderType shaderType;
  int numComponents;
};

const std::vector<Var> programVars = {{"pos", InAttrib, GL_FLOAT, StVector, 3},
                                      {"col", InAttrib, GL_FLOAT, StVector, 3},
                                      {"texCoord", InAttrib, GL_FLOAT, StVector, 2},
                                      {"norm", InAttrib, GL_FLOAT, StVector, 3},
                                      {"proj", InUniform, GL_FLOAT, StMatrix, 4},
                                      {"view", InUniform, GL_FLOAT, StMatrix, 4},
                                      {"model", InUniform, GL_FLOAT, StMatrix, 4},
                                      {"lightPos", InUniform, GL_FLOAT, StVector, 3},
                                      {"lightCol", InUniform, GL_FLOAT, StVector, 3},
                                      {"matDifCol", InUniform, GL_FLOAT, StVector, 3},
                                      {"matSpcCol", InUniform, GL_FLOAT, StVector, 3},
                                      {"shiny", InUniform, GL_FLOAT, StScalar, 1},
                                      {"camPos", InUniform, GL_FLOAT, StVector, 3},
                                      {"samp", InUniform, GL_INT, StScalar, 1},
                                      {"lightFromWorld", InUniform, GL_FLOAT, StMatrix, 4},
                                      {"worldFromLight", InUniform, GL_FLOAT, StMatrix, 4}};

class Prog {
 public:
  union Ui {
    GLuint u;
    GLint i;
  };

  GLuint p;
  std::unordered_map<std::string, Ui> locs;
  // Ui pos, col, texCoord, norm, proj, view, model, lightPos, lightCol, matDifCol, matSpcCol, shiny, camPos, samp;

  Prog() {}

  Prog(const char* baseShaderName) {
    create(baseShaderName);
  }

  Prog(const char* vertexShaderFilename, const char* fragmentShaderFilename) {
    create(vertexShaderFilename, fragmentShaderFilename);
  }

  void create(const char* vertexShaderFilename, const char* fragmentShaderFilename);
  void create(const char* baseShaderName);

  void load(const Scene& scene);

  Ui loc(const std::string& var) const;
};