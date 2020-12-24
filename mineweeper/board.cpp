#include "board.h"
#include "rectangle.h"

// Set the board
void Board::build(int w, int h, int m) {
  if (board != nullptr) {
    delete[] board;
  }
  board = new Tile[w * h];
  width = w;
  height = h;
  numMines = m;
  for (int r = 0; r < height; r++) {
    for (int c = 0; c < width; c++) {
      Tile& t = el(r, c);
      t.revealed = false;
      t.flag = false;
      t.num = 0;
    }
  }
}

// Set the board with the first tile and its radius having no bombs
void Board::initialize(int, int) {
  initialized = true;
}

void Board::reveal(int x, int y) {
  if (!initialized) {
    initialize(x, y);
  }
}

// Given a row and col return the tile in that location
Board::Tile& Board::el(int x, int y) {
  return board[y * width + x];
}

Board::Tile Board::el(int x, int y) const {
  return board[y * width + x];
}