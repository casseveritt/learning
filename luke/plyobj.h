#pragma once

#include <string.h>

#include "geom.h"
#include "linear.h"
#include "shape.h"

using namespace r3;

class Plyobj : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  FILE* plyFile;

  std::string nextLine(FILE* f, int offset = 0);

  void build(FILE* f, Matrix4f m, Vec3f col = Vec3f(0.9f, 0.9f, 0.9f));

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};