#pragma once

#include <vector>

#include "linear.h"

using namespace r3;

class Board {
 public:
  enum State { Uninitialized, Playing, Failed, Won };
  struct Tile {
    int adjMines = 0;
    bool revealed = false;
    bool flagged = false;
    bool isMine = false;
  };
  std::vector<Tile> board;
  int width = 0;
  int height = 0;
  int numMines = 0;
  State state = Uninitialized;

  void build(int inx, int iny, int inb);
  void initialize(int x, int y);
  void checkWin();
  void lostGame();
  void flood(int x, int y);
  void reveal(int x, int y);
  void flag(int x, int y);

  Tile& el(int row, int col);
  Tile el(int row, int col) const;
};