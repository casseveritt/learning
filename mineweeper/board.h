#pragma once

#include "linear.h"

using namespace r3;

class Board {
 public:
  struct Tile {
    int num = 0;
    bool revealed, flag;
  };
  Tile* board = nullptr;
  int width, height, numMines;
  bool initialized = false;

  void build(int inx, int iny, int inb);
  void initialize(int x, int y);
  void reveal(int x, int y);

  Tile& el(int row, int col);
  Tile el(int row, int col) const;
};