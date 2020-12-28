#include "board.h"

#include <stdio.h>
#include <stdlib.h>

// Set the board
void Board::build(int w, int h, int m) {
  board.clear();
  board.resize(w * h);
  width = w;
  height = h;
  numMines = m;
}

// Set the board with the first tile and its radius having no bombs
void Board::initialize(int fx, int fy) {
  state = Playing;
  int minesToPlace = numMines;

  while (minesToPlace > 0) {
    int x = rand() % width;
    int y = rand() % height;
    Tile& t = el(x, y);

    if (t.isMine) {
      continue;
    }

    // We don't place mines on or adjacent to the first clicked location.
    bool adjacentToFirstClick = false;
    for (int yy = std::max(y - 1, 0); yy < std::min(y + 2, height); yy++) {
      for (int xx = std::max(x - 1, 0); xx < std::min(x + 2, width); xx++) {
        adjacentToFirstClick = adjacentToFirstClick || (std::abs(xx - fx) < 2 && std::abs(yy - fy) < 2);
      }
    }
    if (adjacentToFirstClick) {
      continue;
    }

    // Place this mine.
    t.isMine = true;
    minesToPlace--;

    // Increment the adjMines for all adjacent tiles
    for (int yy = std::max(y - 1, 0); yy < std::min(y + 2, height); yy++) {
      for (int xx = std::max(x - 1, 0); xx < std::min(x + 2, width); xx++) {
        Tile& u = el(xx, yy);
        u.adjMines++;
      }
    }
    // Not that it matters, but fix up adjMines for this tile.
    t.adjMines--;
  }
}

void Board::checkWin() {
  int tilesToWin = (width * height) - numMines;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      Tile& t = el(x, y);
      if (!t.isMine && t.revealed) {
        tilesToWin--;
      }
    }
  }
  if (tilesToWin == 0) {
    state = Won;
  }
}

void Board::lostGame() {
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      Tile& t = el(x, y);
      if (t.isMine && !t.flagged) {
        t.revealed = true;
      }
    }
  }
  state = Failed;
}

void Board::flood(int x, int y) {
  Tile& t = el(x, y);
  if (t.revealed) {
    return;
  }
  t.revealed = true;
  t.flagged = false;
  if (t.adjMines == 0) {
    for (int yy = std::max(y - 1, 0); yy < std::min(y + 2, height); yy++) {
      for (int xx = std::max(x - 1, 0); xx < std::min(x + 2, width); xx++) {
        if ((xx != x) || (yy != y)) {
          flood(xx, yy);
        }
      }
    }
  }
}

void Board::reveal(int x, int y) {
  if (state == Uninitialized) {
    initialize(x, y);
  }
  if (state != Playing) {
    return;
  }

  Tile& t = el(x, y);

  if (t.revealed && t.adjMines > 0) {
    int flagCount = 0;
    for (int yy = std::max(y - 1, 0); yy < std::min(y + 2, height); yy++) {
      for (int xx = std::max(x - 1, 0); xx < std::min(x + 2, width); xx++) {
        if (el(xx, yy).flagged) {
          flagCount++;
        }
      }
    }
    if (flagCount == t.adjMines) {
      for (int yy = std::max(y - 1, 0); yy < std::min(y + 2, height); yy++) {
        for (int xx = std::max(x - 1, 0); xx < std::min(x + 2, width); xx++) {
          if (!el(xx, yy).flagged && !el(xx, yy).revealed) {
            reveal(xx,yy);
          }
        }
      }
      checkWin();
    }
  }

  if (t.flagged || t.revealed) {
    return;
  }

  // Sets failed mine click as flagged and revealed to render differently
  if (t.isMine) {
    t.revealed = true;
    t.flagged = true;
    lostGame();
    state = Failed;
    return;
  }  

  flood(x, y);
  checkWin();
}

// Places a flag on UNREVEALED tiles, or takes them off
void Board::flag(int x, int y) {
  Tile& t = el(x, y);
  if (!t.revealed && (state == Playing)) {
    t.flagged = !t.flagged;
  }
}

// Given a row and col return the tile in that location
Board::Tile& Board::el(int x, int y) {
  return board[y * width + x];
}

Board::Tile Board::el(int x, int y) const {
  return board[y * width + x];
}