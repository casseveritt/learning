#pragma once
#include "geom.h"

using namespace r3;

class Cube {
 public:
  // Matrix4f mbase;
  Geom obj;
  Matrix4f scale;

  void appendVert(Matrix4f m, const Vec3f& v, const Vec2f& t);

  void appendSquare(Matrix4f m);

  void build(Matrix4f m);

  void draw(const Scene& scene, Prog p);

  bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection);
};