#pragma once
#include "geom.h"
#include "sphere.h"

class Tetra {
 public:
  int numPoints;
  Vec3f* points = nullptr;
  Vec3f pos;
  Sphere* dots = nullptr;
  void build(int np);
  // float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c);
  void move(Vec3f m);
  void reset();
  void draw(const Scene& scene, Prog p, int i);
};