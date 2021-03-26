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
  };
  struct Poly {
    Vec3i pInd;
    float area;
  };
  int vertSize, faceSize;
  std::vector<Vert> vertices;
  std::vector<Poly> faces;

  float mag(Vec3f vecIn);

  std::string nextLine(FILE* f, int offset = 0);

  void removeEdge(int p0, int p1, int f0, int f1);
  void simplify(int endFaces);

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};