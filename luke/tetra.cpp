#include "tetra.h"

#include <stdio.h>

#include "linear.h"

using namespace r3;

void Tetra::build(int np) {
  numPoints = np;
  auto anchor = new Sphere;  // Unmoving point
  anchor->build(1.0 / (numPoints * 1.25));
  sphs.push_back(anchor);
  sphs.back()->obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
  sphs.back()->obj.modelPose.t = Vec3f(0, 1, 0) / 2;
  for (int i = 0; i < numPoints; i++) {
    auto sph = new Sphere;
    sph->build(1.0 / (numPoints * 1.25));
    sphs.push_back(sph);
    sphs.back()->obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
    Vec3f poi = Vec3f(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1).Normalized();
    sphs.back()->obj.modelPose.t = poi / 2;
  }
}

/*float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c) {
  float d = Dot((b-a).Normalized(), (c-a).Normalized());
  return ToDegrees(acos(d));
}*/

void Tetra::move(Vec3f m) {
  pos = m;
  for (auto sph : sphs) {
    sph->obj.modelPose.t += m;
  }
}

void Tetra::reset() {
  for (int i = 1; i < numPoints; i++) {
    Sphere* sph = sphs[i];
    sph->build(1.0 / (numPoints * 1.25));
    sphs[i] = sph;
    sphs[i]->obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
    Vec3f poi = Vec3f(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1).Normalized();
    sphs[i]->obj.modelPose.t = (poi / 2) + pos;
  }
}

void Tetra::draw(const Scene& scene, Prog p, int iterate) {
  if (iterate == 1) {
    for (int i = 1; i < numPoints; i++) {
      Vec3f push;
      for (int j = 0; j < numPoints; j++) {
        if (i == j) continue;
        Vec3f diff = sphs[i]->obj.modelPose.t - sphs[j]->obj.modelPose.t;
        float len = diff.Length();
        diff.Normalize();
        float scale = 0.1 / (len + 0.5);
        push += diff * scale;
      }
      Sphere* sph = sphs[i];
      sph->build(1.0 / (numPoints * 1.25));
      sphs[i] = sph;
      sphs[i]->obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
      Vec3f poi = (sphs[i]->obj.modelPose.t + push).Normalized();
      sphs[i]->obj.modelPose.t = (poi / 2) + pos;
    }
  }
  for (auto sph : sphs) {
    sph->draw(scene, p);
  }
}

/*bool Tetra::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  Vec3f inter;
  Sphere sph;
  for (int i=0;i<numPoints;i++){
    if (sphs[i].intersect(Vec3f p0, Vec3f p1, Vec3f& inter)) {
      sph = sphs[i]
      sphs[i] = sphs[0];
      sphs[0] = sph;
      intersection = inter;
      return true;
    }
  }
  return false;
}*/