#pragma once
#include "geom.h"

using namespace r3;

class Torus {
 public:
  struct Vert {
    r3::Vec3f col;
    r3::Vec3f pos;
  };
  Geom obj;

  void build(float rad1 = 0.5, float rad2 = 0.25);

  void draw(const Scene& scene, Prog p);
};