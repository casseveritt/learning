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
  int tiles = (width * height) - 9, bombs = numMines;

  for (int i = 0; i < height && bombs > 0; i++) {
    for (int j = 0; j < width && bombs > 0; j++) {
      if (std::abs(j - fx) > 1 || std::abs(i - fy) > 1) {
        float chance = float(bombs) / float(tiles);
        if ((rand() % tiles) + 1 <= bombs) {
          bombs--;
          el(j, i).isMine = true;
          for (int ii = std::max(i - 1, 0); ii < std::min(i + 2, height); ii++) {
            for (int jj = std::max(j - 1, 0); jj < std::min(j + 2, width); jj++) {
              el(jj, ii).adjMines++;
            }
          }
          el(j, i).adjMines--;
        }
        tiles--;
      }
    }
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
    for (int i = 0; i < int(board.size()); i++) {
      if (board[i].isMine && !board[i].flagged) {
        board[i].flagged = true;
      }
    }
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
            reveal(xx, yy);
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
  if (!t.revealed) {
    if (state == Playing) {
      t.flagged = !t.flagged;
    }
  } else {
    reveal(x, y);
  }
}

// Given a row and col return the tile in that location
Board::Tile& Board::el(int x, int y) {
  return board[y * width + x];
}

Board::Tile Board::el(int x, int y) const {
  return board[y * width + x];
}