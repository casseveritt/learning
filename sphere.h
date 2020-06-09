#pragma once
#include "geom.h"

using namespace r3;

class Sphere {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Geom obj;
  float r;

  void build(float radi = 0.5, Vec3f col = Vec3f(0.9f, 0.9f, 0.9f));

  void draw(const Scene& scene, Prog p);

  bool sphereInter(Vec3f rayOrigin, Vec3f rayEnd);
};