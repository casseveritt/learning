#pragma once
#include "learning.h"
#include "linear.h"

struct Scene {
  r3::Matrix4f projMat;
  r3::Posef camPose;
  r3::Posef lightPose;
  r3::Vec3f lightCol;
  r3::Vec3f matDifCol;
  r3::Vec3f matSpcCol;
  r3::Vec3f shiny;
  r3::Vec3f camPos;
};