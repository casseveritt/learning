#pragma once
#include "geom.h"
#include "shape.h"

using namespace r3;

class Square : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  float sqrDime;
  float sqrSize;
  float sideLen;

  void build(float sd, float ss, float s);

  virtual void draw(const Scene& scene, Prog p);

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};