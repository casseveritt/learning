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
    return std::min(a, b);
  }

  int32_t max() const {
    return std::max(a, b);
  }

  int32_t a = -1;
  int32_t b = -1;
};

struct IndexTriple {
  IndexTriple() = default;
  IndexTriple(int32_t i0, int32_t i1, int32_t i2) : a(i0), b(i1), c(i2) {
    make_key();
  }

  uint64_t as_uint64_t() const {
    return k;
  }

  bool operator==(const IndexTriple& rhs) const {
    return k == rhs.k;
  }

  bool operator!=(const IndexTriple& rhs) const {
    return k != rhs.k;
  }

  void make_key() {
    int32_t aa = a;
    int32_t bb = b;
    int32_t cc = c;
    if (aa > bb) {
      std::swap(aa, bb);
    }
    if (bb > cc) {
      std::swap(bb, cc);
    }
    if (aa > bb) {
      std::swap(aa, bb);
    }
    k = (uint64_t(aa & 0x1fffff) << 42) | (uint64_t(bb & 0x1fffff) << 21) | uint64_t(cc & 0x1fffff);
  }

  int32_t a = -1;
  int32_t b = -1;
  int32_t c = -1;
  uint64_t k = 0;
};

namespace std {

template <>
struct hash<IndexPair> {
  std::size_t operator()(const IndexPair& k) const {
    return hash<uint64_t>()(k.as_uint64_t());
  }
};

template <>
struct hash<IndexTriple> {
  std::size_t operator()(const IndexTriple& k) const {
    return hash<uint64_t>()(k.as_uint64_t());
  }
};

}  // namespace std

class Plyobj : public Shape {
 public:
  struct Vert {
    Vec3f col;
    Vec3f norm = Vec3f(0.0f, 0.0f, 0.0f);
    Vec3f pos;
  };

  struct Edge {
    int f0 = -1;
    int f1 = -1;
    int v0 = -1;
    int v1 = -1;
  };

  struct Tri {
    int v[3];
    IndexTriple getIndexTriple() const {
      return IndexTriple(v[0], v[1], v[2]);
    }
  };

  std::vector<Vert> vertices;
  std::vector<Edge> edges;
  std::vector<Tri> tris;
  std::unordered_map<IndexPair, int> vertsToEdgeIndex;
  std::unordered_map<IndexTriple, int> vertsToTriIndex;

  void removeEdge(size_t eInt);
  int findEdgeToRemove();
  void simplify(size_t endFaces);
  void buildTriMap();
  void buildEdgeList(int recursionLevel = 0);

  void build(FILE* f, Matrix4f m);

  virtual void draw(const Scene& scene, Prog p) override;

  virtual bool intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) override;
};