#include "tetra.h"
#include <stdio.h>
#include "linear.h"

using namespace r3;

void Tetra::build(int np) {
  numPoints = np;
  Vec3f points[numPoints];
  points[0] = Vec3f(0.0, 1.0, 0.0);
  Vec3f point(0.0, 1.0, 0.0);
  for (int i = 1; i < numPoints; i++) {
    if (i % 3 == 0)
      point.z = point.z + 1 * i;
    else if (i % 3 == 1)
      point.x = point.x + 1 * i;
    else
      point.y = point.y + 1 * i;
    points[i] = point;
  }
  for (int i = 0; i < numPoints; i++) {
    points[i].Normalized();
  }
  for (int loop = 0; loop < 2000; loop++) {
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
  }
  dots = new Sphere[numPoints];
  for (int i = 0; i < numPoints; i++) {
    dots[i].build(1.0 / (numPoints * 2));
    dots[i].obj.matDifCol = Vec3f(0.2, 0.2, 0.2);
    dots[i].obj.modelPose.t = points[i] / 2;
  }
  /*printf("%f\t",Angle(points[0],points[1],points[2]));
  printf("%f\t",Angle(points[0],points[2],points[3]));
  printf("%f\n",Angle(points[0],points[1],points[3]));
  printf("%f\t",Angle(Vec3f(),points[1],points[2]));
  printf("%f\t",Angle(Vec3f(),points[2],points[3]));
  printf("%f\n",Angle(Vec3f(),points[1],points[3]));*/
}

/*float Tetra::Angle( const Vec3f & a, const Vec3f & b, const Vec3f & c) {
        float d = Dot((b-a).Normalized(), (c-a).Normalized());
        return ToDegrees(acos(d));
}*/

void Tetra::move(Vec3f m) {
  for (int i = 0; i < numPoints; i++) {
    dots[i].obj.modelPose.t = dots[i].obj.modelPose.t + m;
    printf("%f\t%f\t%f\n", dots[i].obj.modelPose.t.x, dots[i].obj.modelPose.t.y, dots[i].obj.modelPose.t.z);
  }
}

void Tetra::draw(const Scene& scene, Prog p) {
  for (int i = 0; i < numPoints; i++) {
    dots[i].draw(scene, p);
  }
}