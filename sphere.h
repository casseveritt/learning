#pragma once
#include "geom.h"
#include "shape.h"

using namespace r3;

class Sphere : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  float r;

  void build(float radi = 0.5, Vec3f col = Vec3f(0.9f, 0.9f, 0.9f));

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};