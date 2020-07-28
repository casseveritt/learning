#include "tetra.h"
#include <stdio.h>
#include "linear.h"

using namespace r3;

void Tetra::build(int np) {
  numPoints = np;
  points = new Vec3f[numPoints];
  points[0] = Vec3f(0.0, 1.0, 0.0);
  for (int i = 1; i < numPoints; i++) {
    points[i] = Vec3f(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1);
    points[i].Normalize();
  }
  /*for (int loop = 0; loop < 2000; loop++) {
    for (int i = 1; i < numPoints; i++) {
      Vec3f push;
      for (int j = 0; j < numPoints; j++) {
        if (i == j) continue;
        Vec3f diff = points[i] - points[j];
        float len = diff.Length();
        diff.Normalize();
        float scale = 0.1 / (len + 0.5);
        push += diff * scale;
      }
      points[i] += push;
      points[i].Normalize();
    }
  }*/
  dots = new Sphere[numPoints];
  for (int i = 0; i < numPoints; i++) {
    dots[i].build(1.0 / (numPoints * 1.25));
    dots[i].obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
    dots[i].obj.modelPose.t = points[i] / 2;
  }
}

/*float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c) {
        float d = Dot((b-a).Normalized(), (c-a).Normalized());
        return ToDegrees(acos(d));
}*/

void Tetra::move(Vec3f m) {
  pos = m;
  for (int i = 0; i < numPoints; i++) {
    dots[i].obj.modelPose.t = dots[i].obj.modelPose.t + m;
  }
}

void Tetra::reset() {
  for (int i = 1; i < numPoints; i++) {
    points[i] = Vec3f(drand48() * 2 - 1, drand48() * 2 - 1, drand48() * 2 - 1);
    points[i].Normalize();
  }
  for (int i = 0; i < numPoints; i++) {
    dots[i].build(1.0 / (numPoints * 1.25));
    dots[i].obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
    dots[i].obj.modelPose.t = points[i] / 2;
    dots[i].obj.modelPose.t += pos;
  }
}

void Tetra::draw(const Scene& scene, Prog p, int iterate) {
  if (iterate == 1) {
    for (int i = 1; i < numPoints; i++) {
      Vec3f push;
      for (int j = 0; j < numPoints; j++) {
        if (i == j) continue;
        Vec3f diff = points[i] - points[j];
        float len = diff.Length();
        diff.Normalize();
        float scale = 0.1 / (len + 0.5);
        push += diff * scale;
      }
      points[i] += push;
      points[i].Normalize();
    }
    for (int i = 0; i < numPoints; i++) {
      dots[i].build(1.0 / (numPoints * 1.25));
      dots[i].obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
      dots[i].obj.modelPose.t = points[i] / 2;
      dots[i].obj.modelPose.t += pos;
    }
  }
  for (int i = 0; i < numPoints; i++) {
    dots[i].draw(scene, p);
  }
}