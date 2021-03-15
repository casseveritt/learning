#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lin.h"
//#include "linear.h"
#include "randmat.h"
#include "Sophus/sophus/se3.hpp"

using namespace Sophus;
using namespace Eigen;

// g++ -I eigen -I Sophus --std=c++11 lintest.cpp -o lintest

void printVec4(const float *v) {
  printf("%.3f, %.3f, %.3f : %.3f\n\n", v[0], v[1], v[2], v[3]);
}

void printMatrix4(const float *m) {
  printf("%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, %.3f\n%.3f, %.3f, %.3f, "
         "%.3f\n%.3f, %.3f, %.3f, %.3f\n\n",
         m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10],
         m[11], m[12], m[13], m[14], m[15]);
}

lVec3f ToL(const Vector3f &svec) {
  return *reinterpret_cast<const lVec3f *>(&svec);
}

Vector3f ToS(const lVec3f &lvec) {
  return *reinterpret_cast<const Vector3f *>(&lvec);
}

Vector4f ToS(const lVec4f &lvec) {
  return *reinterpret_cast<const Vector4f *>(&lvec);
}

Matrix4f ToS(const lMatrix4f &lmat) {
  return *reinterpret_cast<const Matrix4f *>(&lmat);
}

int main(int argc, char **argv) {

  float angle0 = 35.0f;
  float angle1 = 73.0f;
  lVec3f axis0(1.0f, 1.0f, 0.0f);
  lVec3f axis1(1.0f, 0.0f, 1.0f);
  // lMatrix4f lRotMat;
  // lRotMat = lRotMat.Rotate(axis1, lToRadians(angle1));
  // Matrix4f r3RotMat = Quaternionf(ToR3(axis1),
  // lToRadians(angle1)).GetMatrix4();
  lVec3f lPoint0(2.0f, 3.0f, 5.0f);
  lVec3f lPoint1(7.0f, 1.0f, 6.0f);
  Sophus::Vector3f sPoint0 = ToS(lPoint0);
  Sophus::Vector3f sPoint1 = ToS(lPoint1);
  printf("Point 0| %.3f, %.3f, %.3f\n", lPoint0.x, lPoint0.y, lPoint0.z);
  printf("Point 1| %.3f, %.3f, %.3f\n", lPoint1.x, lPoint1.y, lPoint1.z);
  printf("Quat 0 Axis and Angle| %.3f, %.3f, %.3f : %.3f\n", axis0.x, axis0.y, axis0.z, angle0);
  printf("Quat 1 Axis and Angle| %.3f, %.3f, %.3f : %.3f\n\n", axis1.x, axis1.y, axis1.z, angle1);
  // printf("Quat 1 Matrix4|\n");
  // printMatrix4(&lRotMat.lM[0]);
  // printf("\n");

  lQuaternionf lQuat0(axis0, lToRadians(angle0));
  lQuaternionf lQuat1(axis1, lToRadians(angle1));
  // lQuaternionf lQuat1(lRotMat);
  lQuaternionf lMultQuat = lQuat0 * lQuat1;
  Quaternionf sQuat0(lToRadians(angle0), axis0.x, axis0.y, axis0.z);
  Quaternionf sQuat1(lToRadians(angle1), axis1.x, axis1.y, axis1.z);
  // sQuaternionf sQuat1(sRotMat);
  Quaternion<float> sMultQuat = sQuat0 * sQuat1;
  lPoint0 = lQuat0 * lPoint0;
  lPoint1 = lQuat1 * lPoint1;
  sPoint0 = sQuat0 * sPoint0;
  sPoint1 = sQuat1 * sPoint1;

  lVec3f p0Diff = lPoint0 - ToL(sPoint0), p1Diff = lPoint1 - ToL(sPoint1);

  printf("l Quat 0| %.3f, %.3f, %.3f : %.3f\n", lQuat0.x, lQuat0.y, lQuat0.z, lQuat0.w);
  //printf("s Quat 0| %.3f, %.3f, %.3f : %.3f\n", sQuat0.x, sQuat0.y, sQuat0.z, sQuat0.w);
  printf("l Quat 1| %.3f, %.3f, %.3f : %.3f\n", lQuat1.x, lQuat1.y, lQuat1.z, lQuat1.w);
  //printf("s Quat 1| %.3f, %.3f, %.3f : %.3f\n\n", sQuat1.x, sQuat1.y, sQuat1.z, sQuat1.w);
  printf("l Mult Quat| %.3f : %.3f, %.3f, %.3f\n", lMultQuat.w, lMultQuat.x, lMultQuat.y, lMultQuat.z);
  //printf("s Mult Quat| %.3f : %.3f, %.3f, %.3f\n\n", sMultQuat.x, sMultQuat.y, sMultQuat.z, sMultQuat.w);
  printf("Point 0 Trans Diff| %.3f, %.3f, %.3f\n", p0Diff.x , p0Diff.y, p0Diff.z);
  printf("Point 1 Trans Diff| %.3f, %.3f, %.3f\n", p1Diff.x , p1Diff.y, p1Diff.z);

  return 0;
}
