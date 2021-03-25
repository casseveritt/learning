#pragma once

#include <string.h>

#include "geom.h"
#include "linear.h"
#include "shape.h"

using namespace r3;

class Plyobj : public Shape {
 public:
  struct Vert {
    Vec3f norm = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f pos;
    std::vector<int> defFaces;
  };
  struct Poly {
    Vec3i points;
    float area;
  };

  std::string nextLine(FILE* f, int offset = 0);

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};