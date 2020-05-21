#include "cube.h"

using namespace r3;
// cube generator

static void appendVert(Geom &obj, const Matrix4f &m, const Vec3f &v,
                       const Vec2f &t) {
  Vec3f xv = m * v;
  obj.texCoord(t.x, t.y);
  obj.color(t.x, t.y, 0.0);
  obj.position(xv.x, xv.y, xv.z);
}

static void appendSquare(Geom &obj, Matrix4f &m) {
  appendVert(obj, m, Vec3f(-1, -1, 1), Vec2f(0, 0));
  appendVert(obj, m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(obj, m, Vec3f(-1, 1, 1), Vec2f(0, 1));

  appendVert(obj, m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(obj, m, Vec3f(1, 1, 1), Vec2f(1, 1));
  appendVert(obj, m, Vec3f(-1, 1, 1), Vec2f(0, 1));
}

void makeCube(Geom &obj, const Matrix4f &mbase) {

  Matrix4f m = mbase;
  Posef p;

  obj.begin(GL_TRIANGLES);
  obj.color(0.1f, 0.05f, 0.95f);

  obj.normal(0.0, 0.0, 1.0);
  appendSquare(obj, m);
  p.r = Quaternionf(Vec3f(1, 0, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(0.0, -1.0, 0.0);
  obj.color(0.1f, 0.95f, 0.05f);
  appendSquare(obj, m);
  m *= p.GetMatrix4();

  obj.normal(0.0, 0.0, -1.0);
  obj.color(0.95f, 0.05f, 0.05f);
  appendSquare(obj, m);
  m *= p.GetMatrix4();

  obj.normal(0.0, 1.0, 0.0);
  obj.color(0.1f, 0.95f, 0.95f);
  appendSquare(obj, m);
  m *= p.GetMatrix4();
  p.r = Quaternionf(Vec3f(0, 1, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  obj.normal(1.0, 0.0, 0.0);
  obj.color(0.95f, 0.05f, 0.95f);
  appendSquare(obj, m);
  m *= p.GetMatrix4();
  m *= p.GetMatrix4();

  obj.normal(-1.0, 0.0, 0.0);
  obj.color(0.95f, 0.95f, 0.05f);
  appendSquare(obj, m);

  obj.end();
}