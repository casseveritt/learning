#include "cube.h"

using namespace r3;
// cube generator

void Cube::appendVert(Matrix4f m, const Vec3f &v, const Vec2f &t) {
  Vec3f xv = m * v;
  obj.texCoord(t.x, t.y);
  obj.position(xv.x, xv.y, xv.z);
}

void Cube::appendSquare(Matrix4f m) {
  appendVert(m, Vec3f(-1, -1, 1), Vec2f(0, 0));
  appendVert(m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(m, Vec3f(-1, 1, 1), Vec2f(0, 1));

  appendVert(m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(m, Vec3f(1, 1, 1), Vec2f(1, 1));
  appendVert(m, Vec3f(-1, 1, 1), Vec2f(0, 1));
}

void Cube::build(Matrix4f m) {

  Posef p;

  obj.begin(GL_TRIANGLES);
  obj.color(0.1f, 0.05f, 0.95f);

  obj.normal(0.0, 0.0, 1.0);
  appendSquare(m);
  p.r = Quaternionf(Vec3f(1, 0, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(0.0, -1.0, 0.0);
  obj.color(0.1f, 0.95f, 0.05f);
  appendSquare(m);
  m *= p.GetMatrix4();

  obj.normal(0.0, 0.0, -1.0);
  obj.color(0.95f, 0.05f, 0.05f);
  appendSquare(m);
  m *= p.GetMatrix4();

  obj.normal(0.0, 1.0, 0.0);
  obj.color(0.1f, 0.95f, 0.95f);
  appendSquare(m);
  m *= p.GetMatrix4();
  p.r = Quaternionf(Vec3f(0, 1, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(1.0, 0.0, 0.0);
  obj.color(0.95f, 0.05f, 0.95f);
  appendSquare(m);
  m *= p.GetMatrix4();
  m *= p.GetMatrix4();

  obj.normal(-1.0, 0.0, 0.0);
  obj.color(0.95f, 0.95f, 0.05f);
  appendSquare(m);

  obj.end();
}

void Cube::draw(const Scene &scene, Prog p) { obj.draw(scene, p); }