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
    int f0 = -1;
    int f1 = -1;
    int v0 = -1;
    int v1 = -1;
    float len;
    int influencer = -1;
  };
  struct Tri {
    int v[3];
    float area;
  };
  int vertSize, faceSize, edgeSize = 0;
  std::vector<Vert> vertices;
  std::vector<Edge> edges;
  std::vector<Tri> tris;
  std::unordered_map<uint64_t, int> vertsToEdgeIndex;

  float mag(Vec3f vecIn);

  std::string nextLine(FILE* f, int offset = 0);

  void removeEdge(int eInt);
  void simplify(int endFaces);
  void buildEdgeList();

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};