#pragma once
#include "learning.h"
#include "linear.h"

struct Scene {
  r3::Matrix4f projMat;
  r3::Posef camPose;
  r3::Vec3f lightPos;
  r3::Vec3f lightCol;
  r3::Vec3f matCol;
};