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
    int f0 = -1, f1 = -1, v0 = -1, v1 = -1;
    int influencer = -1;
  };
  struct Tri {
    int v[3], probs;
    float area;
  };
  int vertSize, faceSize;
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