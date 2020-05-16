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
  r3::Posef modelPose;
  GLenum primType;
  struct Vertex {
    r3::Vec3f color;
    r3::Vec3f normal;
    r3::Vec3f position;
  };
  r3::Vec3f matDifCol = r3::Vec3f(0.9f, 0.9f, 0.9f);
  r3::Vec3f matSpcCol = r3::Vec3f(0.9f, 0.9f, 0.9f);
  float shiny = 10.0;
  Vertex v;
  std::vector<Vertex> verts;

  Geom();

  void begin(GLenum prim);

  void end();

  void color(float r, float g, float bl);

  void normal(float x, float y, float z);

  void normal(r3::Vec3f norm);

  void position(float x, float y, float z);

  void position(r3::Vec3f cords);

  void draw(const Scene &scene, Prog p);
};