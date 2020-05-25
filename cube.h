#pragma once
#include "geom.h"

class Cube{
public:
	Geom cubeObj;
	const r3::Matrix4f &mbase;

	void appendVert(const Matrix4f &m, const Vec3f &v, const Vec2f &t);

	void appendSquare(Matrix4f &m);

	void makeCube(const Matrix4f &mbase);
}