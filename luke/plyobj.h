#pragma once

#include <string.h>
#include <unordered_map>

#include "geom.h"
#include "linear.h"
#include "shape.h"

using namespace r3;

class Plyobj : public Shape {
 public:
  struct Vert {
    Vec3f norm = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f pos;
  };
  struct Edge {
    float len;
    int f0, f1, p0, p1, influencer = -1;
  };
  struct Poly {
    Vec3i pInd;
    float area;
  };
  int vertSize, faceSize, edgeSize = 0;
  std::vector<Vert> vertices;
  std::vector<Edge> edges;
  std::vector<Poly> faces;

  float mag(Vec3f vecIn);

  std::string nextLine(FILE* f, int offset = 0);

  void removeEdge(int f0, int f1, int p0, int p1);
  void simplify(int endFaces);
  void findEdges();

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};