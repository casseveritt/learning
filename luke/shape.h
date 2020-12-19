#pragma once
#ifndef SHAPEH
#define SHAPEH

#include "linear.h"

class Shape {
 public:
  Geom obj;
  int form = -1;  // -1 = None - 0,1,2,3 - Square, Cube, Sphere, Torus
  virtual void draw(const Scene& scene, Prog p) = 0;
  virtual bool intersect(r3::Vec3f p0, r3::Vec3f p1, r3::Vec3f& intersection) = 0;
};
#endif