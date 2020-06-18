#pragma once
#include "geom.h"

using namespace r3;

class Torus {
 public:
  struct Vert {
    r3::Vec3f col;
    r3::Vec3f pos;
  };
  float bigr;
  float littler;
  Geom obj;

  void build(float rad1 = 0.5, float rad2 = 0.25);

  void draw(const Scene& scene, Prog p);

  float eval(Vec3f p0);

  bool intersect(Vec3f p0, Vec3f p1);

  bool directIntersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};