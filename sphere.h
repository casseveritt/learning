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
  Geom sphObj;

  void build(float radi = 0.5);

  void draw(const Scene &scene, Prog p);
};