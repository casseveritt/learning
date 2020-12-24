#pragma once

#include "board.h"
#include "linear.h"

struct Renderer {
  r3::Vec3i boardDim;
  r3::Vec2d prevPos;
  r3::Vec2d diffPos;
  virtual void Init() = 0;
  virtual void Draw(const Board& b) = 0;
  virtual void SetWindowSize(int w, int h) = 0;
  // virtual void Click(int w, int h) = 0;
  virtual void SetCursorPos(r3::Vec2d cursorPos) = 0;
};
Renderer* CreateRenderer();
