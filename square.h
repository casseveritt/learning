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
  float sideLen;

  void build(float sd, float ss, float s);

  void draw(const Scene& scene, Prog p);

  bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};