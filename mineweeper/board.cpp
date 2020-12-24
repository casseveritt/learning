#include "board.h"

#include "stdlib.h"

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
      t.flagged = false;
      t.adjMines = 0;
    }
  }
}

// Set the board with the first tile and its radius having no bombs
void Board::initialize(int x, int y) {
  state = Playing;
  int minesToPlace = numMines;

  while (minesToPlace > 0) {
    int r = rand() % width;
    int c = rand() % height;
    Tile& t = el(c, r);

    bool inClickRadius = false;
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if ((x + i < width && x + i >= 0) && (y + j < height && y + j >= 0)) {
          if (c == x + i && r == y + j) {
            inClickRadius = true;
          }
        }
      }
    }
    if (t.isMineS || inClickRadius) {
      continue;
    }
    t.isMine = true;

    minesToPlace--;

    for (int rr = r - 1; rr < r + 2; rr++) {
      for (int cc = c - 1; cc < c + 2; cc++) {
        if (rr >= 0 && rr < height && cc >= 0 && cc < width) {
          Tile& u = el(cc, rr);
          u.adjMines++;
        }
      }
    }
    t.adjMines--;
  }
}

void Board::checkWin() {
  int tilesToWin = (width * height) - numMines;
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      if (!el(x, y).isMine && el(x, y).revealed) {
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
      if (el(x, y).isMine && !el(x, y).flagged) {
        el(x, y).revealed = true;
      }
    }
  }
  state = Failed;
}

void Board::flood(int x, int y) {
  if (el(x, y).adjMines == 0 && !el(x, y).revealed) {
    el(x, y).revealed = true;
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        int xx = x + i;
        int yy = y + j;
        if ((xx < width && xx >= 0) && (yy < height && yy >= 0)) {
          flood(xx, yy);
        }
      }
    }
  } else {
    el(x, y).revealed = true;
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

  if (t.revealed && (t.adjMines > 0)) {
    int flagCount = 0;
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; j++) {
        if ((x + i < width && x + i >= 0) && (y + j < height && y + j >= 0)) {
          if (el(x + i, y + j).flagged) {
            flagCount++;
          }
        }
      }
    }
    if (flagCount == t.adjMines) {
      for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
          if ((x + i < width && x + i >= 0) && (y + j < height && y + j >= 0)) {
            if (!el(x + i, y + j).flagged && !el(x + i, y + j).revealed) {
              el(x + i, y + j).revealed = true;
            }
          }
        }
      }
      checkWin();
    }
  }

  if (t.flagged || t.revealed) {
    return;
  }

  if (t.adjMines == 0 && !t.isMine) {
    flood(x, y);
    checkWin();
    return;
  }

  t.revealed = true;
  checkWin();

  if (t.isMine) {
    t.flagged = true;
    lostGame();
    state = Failed;
    return;
  }
}

// Places a flag on UNREVEALED tiles, or takes them off
void Board::flag(int x, int y) {
  if (!el(x, y).revealed) {
    el(x, y).flagged = (!el(x, y).flagged);
  }
}

// Given a row and col return the tile in that location
Board::Tile& Board::el(int x, int y) {
  return board[y * width + x];
}

Board::Tile Board::el(int x, int y) const {
  return board[y * width + x];
}