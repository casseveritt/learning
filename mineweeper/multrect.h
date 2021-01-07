#pragma once

#include <vector>
#include "rectangle.h"

class MultRect {
 public:
  Geom obj;
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  struct Rect {
    Rect(float x, float y) : x0(x), y0(y) {}
    float x0, y0;
  };
  float s0, s1;

  void build(const std::vector<Rect>& rects);

  void draw(const Scene& scene, Prog p);
};