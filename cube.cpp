#include "cube.h"

using namespace r3;
// cube generator

void Cube::appendVert(Matrix4f m, const Vec3f& v, const Vec2f& t) {
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
  form = 1;
  scale = m;
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

void Cube::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Cube::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  Matrix4f worldFromObj = obj.modelPose.GetMatrix4() * scale;
  Matrix4f objFromWorld = worldFromObj.Inverted();
  p0 = objFromWorld * p0;
  p1 = objFromWorld * p1;
  Planef x0(Vec3f(-1, 0, 0), -1.0f);
  Planef x1(Vec3f(1, 0, 0), -1.0f);
  Planef y0(Vec3f(0, -1, 0), -1.0f);
  Planef y1(Vec3f(0, 1, 0), -1.0f);
  Planef z0(Vec3f(0, 0, -1), -1.0f);  // All Pointed In
  Planef z1(Vec3f(0, 0, 1), -1.0f);
  Planef planes[] = {x0, x1, y0, y1, z0, z1};
  bool success = true;
  for (int i = 0; success && i < 6; i++) {
    Planef& plane = planes[i];
    float d0 = plane.Distance(p0);
    float d1 = plane.Distance(p1);
    if (d0 < 0 && d1 < 0) {
      success = false;
      break;
    } else if (d0 > 0 && d1 > 0) {
      continue;
    }
    bool swapped = (d1 < 0);
    if (swapped) {
      std::swap(d0, d1);
      std::swap(p0, p1);
    }
    float f = d1 / (d1 - d0);
    Vec3f p = p0 * f + p1 * (1.0 - f);
    p0 = p;
    if (swapped) {
      std::swap(d0, d1);
      std::swap(p0, p1);
    }
    d0 = plane.Distance(p0);
    d1 = plane.Distance(p1);
  }
  if (success) {
    intersection = worldFromObj * p0;
  }
  return success;
}