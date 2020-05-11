#pragma once
#include "learning.h"
#include "linear.h"
#include "prog.h"
#include "scene.h"
#include <vector>

class Geom {

public:
  GLuint b;
  GLint pos_loc, col_loc, proj_loc, view_loc, model_loc, norm_loc;
  Posef modelPose;
  GLenum primType;
  struct Vertex {
    r3::Vec3f color;
    r3::Vec3f normal;
    r3::Vec3f position;
  };
  Vertex v;
  std::vector<Vertex> verts;

  Geom();

  void begin(GLenum prim);

  void end();

  void color(float r, float g, float bl);

  void normal(float x, float y, float z);

  void normal(Vec3f norm);

  void position(float x, float y, float z);

  void position(Vec3f cords);

  void draw(const Scene & scene, Prog p);
};