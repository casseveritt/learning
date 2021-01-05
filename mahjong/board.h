#pragma once

#include <cstdint>
#include <vector>

#include "linear.h"

using namespace r3;

class Board {
 public:
  enum State { Uninitialized, Playing, Failed, Won, selectedOneTile, };
  struct Tile {
    int numSelected = 0;
    int adjMines = 0;
    bool flagged = false;
    bool isMine = false;
    bool selected = false;
    bool removed = false;
  };
  std::vector<Tile> board;
  int width = 0;
  int height = 0;
  int numMines = 0;
  int numSelected = 0;
  State state = Uninitialized;

  void build(int inx, int iny, int inb);
  void initialize(int x, int y);
  void checkWin();
  void lostGame();
  void flood(int x, int y);
  void reveal(int x, int y);
  void flag(int x, int y);

  Tile& el(int x, int y);
  Tile el(int x, int y) const;
};