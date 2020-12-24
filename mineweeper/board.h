#pragma once

#include "linear.h"

using namespace r3;

class Board {
 public:
  enum State {
    Uninitialized,
    Playing,
    Failed,
    Won,
  };
  struct Tile {
    int adjMines = 0;
    bool revealed, flagged, isMine;
  };
  Tile* board = nullptr;
  int width, height, numMines;
  State state = Uninitialized;

  void build(int inx, int iny, int inb);
  void initialize(int x, int y);
  void reveal(int x, int y);

  Tile& el(int row, int col);
  Tile el(int row, int col) const;
};