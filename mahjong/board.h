#pragma once

#include <cstdint>
#include <vector>

#include "linear.h"

using namespace r3;

class Board {
 public:
  enum State { Uninitialized, Playing, Failed, Won, selectedOneTile, };
  struct Tile {
    int adjMines = 0;
    bool flagged = false;
    bool isMine = false;
    bool removed = false;
    int tileValue;
  };
  std::vector<Tile> board;
  int width = 0;
  int height = 0;
  int numMines = 0;
  int tileValue;
  State state = Uninitialized;
  Tile * selected = nullptr;

  void build(int inx, int iny, int inb);
  void initialize(int x, int y);
  void checkWin();
  void lostGame();
  void flood(int x, int y);
  void select(int x, int y);
  void flag(int x, int y);

  Tile& el(int x, int y);
  Tile el(int x, int y) const;
};