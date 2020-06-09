#pragma once
#include "geom.h"

using namespace r3;

class Square {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Geom obj;
  float sqrDime;
  float sqrSize;

  void build(float sd, float ss);

  void draw(const Scene& scene, Prog p);
};