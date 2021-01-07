#pragma once

#include "linear.h"

struct Renderer {
  float fovy = 70.0f;
  r3::Posef camera;
  r3::Vec3f intLoc;
  int width, height;
  bool intersect = false, trackCamera = true;
  int iterate = 0;
  virtual void Init() = 0;
  virtual void Draw() = 0;
  virtual void SetCursorPos(r3::Vec2d cursorPos) = 0;
  virtual void ResetSim() = 0;
  virtual void RayInWorld(r3::Vec3f& nIW3, r3::Vec3f& fIW3) = 0;
  virtual void Intersect() = 0;
  virtual void Drag(r3::Vec3f newPos) = 0;
};
Renderer* CreateRenderer();
