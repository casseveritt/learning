#include "cube.h"

using namespace r3;
// cube generator

void Cube::appendVert(const Matrix4f &m, const Vec3f &v, const Vec2f &t) {
  Vec3f xv = m * v;
  cubeObj.texCoord(t.x, t.y);
  cubeObj.color(t.x, t.y, 0.0);
  cubeObj.position(xv.x, xv.y, xv.z);
}

void Cube::appendSquare(Matrix4f &m) {
  appendVert(m, Vec3f(-1, -1, 1), Vec2f(0, 0));
  appendVert(m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(m, Vec3f(-1, 1, 1), Vec2f(0, 1));

  appendVert(m, Vec3f(1, -1, 1), Vec2f(1, 0));
  appendVert(m, Vec3f(1, 1, 1), Vec2f(1, 1));
  appendVert(m, Vec3f(-1, 1, 1), Vec2f(0, 1));
}

void Cube::makeCube(const Matrix4f &mbase) {

  Matrix4f m = mbase;
  Posef p;

  cubeObj.begin(GL_TRIANGLES);
  cubeObj.color(0.1f, 0.05f, 0.95f);

  cubeObj.normal(0.0, 0.0, 1.0);
  appendSquare(m);
  p.r = Quaternionf(Vec3f(1, 0, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  cubeObj.normal(0.0, -1.0, 0.0);
  cubeObj.color(0.1f, 0.95f, 0.05f);
  appendSquare(m);
  m *= p.GetMatrix4();

  cubeObj.normal(0.0, 0.0, -1.0);
  cubeObj.color(0.95f, 0.05f, 0.05f);
  appendSquare(m);
  m *= p.GetMatrix4();

  cubeObj.normal(0.0, 1.0, 0.0);
  cubeObj.color(0.1f, 0.95f, 0.95f);
  appendSquare(m);
  m *= p.GetMatrix4();
  p.r = Quaternionf(Vec3f(0, 1, 0), ToRadians(90.0f));
  m *= p.GetMatrix4();

  cubeObj.normal(1.0, 0.0, 0.0);
  cubeObj.color(0.95f, 0.05f, 0.95f);
  appendSquare(m);
  m *= p.GetMatrix4();
  m *= p.GetMatrix4();

  cubeObj.normal(-1.0, 0.0, 0.0);
  cubeObj.color(0.95f, 0.95f, 0.05f);
  appendSquare(m);

  cubeObj.end();
}