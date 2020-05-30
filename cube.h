#pragma once
#include "geom.h"

using namespace r3;

class Cube {
 public:
  // Matrix4f mbase;
  Geom obj;

  void appendVert(Matrix4f m, const Vec3f& v, const Vec2f& t);

  void appendSquare(Matrix4f m);

  void build(Matrix4f m);

  void draw(const Scene& scene, Prog p);
};