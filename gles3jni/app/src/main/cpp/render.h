#pragma once

#include "board.h"
#include "linear.h"

struct Renderer {
  r3::Vec3i boardDim;
  r3::Vec2d prevPos;
  r3::Vec2d diffPos;
  virtual void Init() = 0;
  virtual void Draw() = 0;
  virtual void SetWindowSize(int w, int h) = 0;
  virtual void SetCursorPos(r3::Vec2d cursorPos) = 0;
  virtual void Touch(float x, float y, int type, int index) = 0;
};
Renderer* CreateRenderer();
