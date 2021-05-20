#pragma once
#include "geom.h"

using namespace r3;

class Rectangle {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Geom obj;
  float s0, s1;

  void build(float side0, float side1, Vec3f rCol = Vec3f(0.9f, 0.9f, 0.9f));

  void draw(const Scene& scene, Prog p);

  bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};