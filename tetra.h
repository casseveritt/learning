#pragma once
#include "geom.h"
#include "sphere.h"

class Tetra {
 public:
  int numPoints;
  Sphere* dots;
  void build(int np);
  // float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c);
  void move(Vec3f m);
  void draw(const Scene& scene, Prog p);
};