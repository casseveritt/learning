#pragma once
#include "geom.h"
#include "shape.h"

using namespace r3;

class Rectangle : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  float s0, s1;

  void build(float side0, float side1, Vec3f rCol = Vec3f(0.9f, 0.9f, 0.9f));

  virtual void draw(const Scene& scene, Prog p);

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};