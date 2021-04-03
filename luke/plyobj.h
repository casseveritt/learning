#pragma once

#include <string.h>

#include <unordered_map>

#include "geom.h"
#include "linear.h"
#include "shape.h"

using namespace r3;


struct IndexPair {
  IndexPair() = default;
  IndexPair(int32_t i0, int32_t i1) : a(i0), b(i1) {}

  uint64_t as_uint64_t() const {
    return (uint64_t(min()) << 32) | uint64_t(max());
  }

  bool operator==(const IndexPair& rhs) const {
    return as_uint64_t() == rhs.as_uint64_t();
  }

  int32_t min() const {
    return std::min(a,b);
  }

  int32_t max() const {
    return std::max(a,b);
  }

  int32_t a = -1;
  int32_t b = -1;
};

namespace std {

template <>
struct hash<IndexPair> {
  std::size_t operator()(const IndexPair& k) const
  {
    return hash<uint64_t>()(k.as_uint64_t());
  }
};

}

class Plyobj : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f pos;
  };

  struct Edge {
    float len;
    int f0 = -1;
    int f1 = -1;
    int v0 = -1;
    int v1 = -1;
    int influencer = -1;
  };

  struct Tri {
    int v[3];
  };

  int vertSize, faceSize;
  std::vector<Vert> vertices;
  std::vector<Edge> edges;
  std::vector<Tri> tris;
  std::unordered_map<IndexPair, int> vertsToEdgeIndex;

  void removeEdge(int eInt);
  int findShortestEdge();
  void simplify(int endFaces);
  void buildEdgeList();

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};