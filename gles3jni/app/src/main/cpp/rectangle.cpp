#include "rectangle.h"

using namespace r3;

void Rectangle::build(float side0, float side1, Vec3f rCol) {
  s0 = side0;
  s1 = side1;
  obj.begin(GL_TRIANGLE_STRIP);
  obj.normal(0, 0, -1);
  obj.color(rCol.x, rCol.y, rCol.z);

  obj.texCoord(0.0f, 1.0f);
  obj.position(0.0f, 0.0f, 0.0f);
  obj.texCoord(0.0f, 0.0f);
  obj.position(0.0f, s1, 0.0f);
  obj.texCoord(1.0f, 1.0f);
  obj.position(s0, 0.0f, 0.0f);
  obj.texCoord(1.0f, 0.0f);
  obj.position(s0, s1, 0.0f);
  obj.end();
}

void Rectangle::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Rectangle::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  Planef plane(Vec3f(0, 0, 1), 0.0f);
  plane.Transform(obj.modelPose.GetMatrix4());
  Vec3f p0ips = p0 + obj.modelPose.t;
  Vec3f p1ips = p1 + obj.modelPose.t;
  Linef ray(p0ips, p1ips);
  bool hit = plane.Intersect(ray, intersection);
  Vec3f i = intersection;
  if (hit == true && i.x < s0 / 2 && i.x > (s0 / 2) * -1 && i.z < s0 / 2 && i.z > (s0 / 2) * -1) {
    intersection -= obj.modelPose.t;
  } else {
    hit = false;
  }
  return hit;
}