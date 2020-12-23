#pragma once

#include "linear.h"

using namespace r3;

class Board {
public:
	struct Tile{
		int num = 0;
		bool revealed;
	};
	Tile* board;
	Vec3i boardD;

	void build(Vec3i dimensions);

	//void Draw(int w, int h);

	//void Intersect(int w, int h, Vec2f intPoint);

	Tile &el(int row, int col);
  	Tile el(int row, int col) const;
};