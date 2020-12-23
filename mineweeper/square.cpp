#include "square.h"

using namespace r3;

void Square::build(float sd, float ss, float s) {
  form = 0;
  sqrDime = sd;
  sqrSize = ss;
  sideLen = s;
  obj.begin(GL_TRIANGLE_STRIP);
  obj.normal(0, 1, 0);
  obj.color(0.0f, 0.0f, 0.0f);
  obj.texCoord(0.0f, 0.0f);
  obj.position(-sqrDime, 0.0f, -sqrDime);
  obj.texCoord(0.0f, sqrSize);
  obj.position(-sqrDime, 0.0f, sqrDime);
  obj.texCoord(sqrSize, 0.0f);
  obj.position(sqrDime, 0.0f, -sqrDime);
  obj.texCoord(sqrSize, sqrSize);
  obj.position(sqrDime, 0.0f, sqrDime);
  obj.end();
}

void Square::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Square::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  Planef plane(Vec3f(0, 1, 0), 0.0f);
  plane.Transform(obj.modelPose.GetMatrix4());
  Vec3f p0ips = p0 + obj.modelPose.t;
  Vec3f p1ips = p1 + obj.modelPose.t;
  Linef ray(p0ips, p1ips);
  bool hit = plane.Intersect(ray, intersection);
  Vec3f i = intersection;
  if (hit == true && i.x < sideLen / 2 && i.x > (sideLen / 2) * -1 && i.z < sideLen / 2 && i.z > (sideLen / 2) * -1) {
    intersection -= obj.modelPose.t;
  } else {
    hit = false;
  }
  return hit;
}