#pragma once

#include "linear.h"

struct Renderer {
  float fovy = 60.0f;
  r3::Vec2d prevPos;
  r3::Vec2d diffPos;
  bool mode1;  //, clickRay;
  float rad = 2.5;
  float theta = 0.0;
  bool intersect = false;
  int iterate = 0;
  virtual void Init() = 0;
  virtual void Draw() = 0;
  virtual void SetWindowSize(int w, int h) = 0;
  virtual void SetCursorPos(r3::Vec2d cursorPos) = 0;
  virtual void ResetSim() = 0;
  virtual void RayInWorld(r3::Vec2d currPos, int width, int height, r3::Vec3f* nIW3, r3::Vec3f* fIW3) = 0;
  virtual void Intersect(r3::Vec3f nIW3, r3::Vec3f fIW3) = 0;
};
Renderer* CreateRenderer();
