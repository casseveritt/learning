#pragma once

#include "board.h"
#include "rectangle.h"

void Board::build(Vec3i dimensions) {
	boardD = dimensions;
	int tiles = (boardD.x * boardD.y);
	int bombs = boardD.z;
	for(int r=0;r<boardD.y;r++) {
		for(int c=0;c<boardD.x;c++) {
			Tile t;
			t.revealed = true; // Change in an actual game
			if(rand()%tiles <= bombs) { // Chance of being a bomb
				t.num = -1;
				for(int i=-1;i<2;i++) { // Increment non-bomb tiles in radius
					for(int j=-1;j<2;j++) {
						if(el(r+i, c+j).num != -1) {
							el(r+i, c+j).num++;
						}
					}
				}
				bombs--;
			}
			tiles--;
		}
	}
}

//void Board::Draw(int w, int h) {}

//void Board::Intersect(int w, int h, Vec2f intPoint) {}

Tile &Board::el(int row, int col) {
	if((row < boardD.y) || (col < boardD.x)) {
		return board[(row * boardD.y) + col];
	}
}

Tile Board::el(int row, int col) const {
	if((row < boardD.y) || (col < boardD.x)) {
		return board[(row * boardD.y) + col];
	}
}