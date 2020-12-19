#pragma once
#include "geom.h"
#include "shape.h"
#include "sphere.h"

class Tetra {
 public:
  int numPoints;
  Vec3f pos;
  std::vector<Sphere*> sphs;
  void build(int np);
  // float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c);
  void move(Vec3f m);
  void reset();
  void draw(const Scene& scene, Prog p, int i);
  // virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};