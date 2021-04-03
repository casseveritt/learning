#include "plyobj.h"

using namespace r3;
using namespace std;

int edgeSize;

namespace {

string nextLine(FILE* f, int offset = 0) {
  if (offset > 0) fseek(f, offset, SEEK_CUR);
  string lineOut;
  char buffer[1];
  bool read = true;
  while (read) {
    fread(buffer, 1, 1, f);
    if (buffer[0] == '\n')
      read = false;
    else
      lineOut.append(1, buffer[0]);
  }
  return lineOut;
}

}  // namespace

void Plyobj::removeEdge(int eInt) {
  Edge e = edges[eInt];
  tris[e.f0] = tris[faceSize - 1];
  tris[e.f1] = tris[faceSize - 2];
  faceSize -= 2;
  tris.resize(faceSize);
  for (int i = 0; i < faceSize; i++) {
    Tri pol = tris[i];
    for (int j = 0; j < 3; j++) {
      if (pol.v[j] == e.v1) {
        tris[i].v[j] = e.v0;
      } else {
        vertices[tris[i].v[j]].col = Vec3f(1.0f, 0.0f, 0.0f);
      }
    }
  }
}

float vertDist(Vec3f v0, Vec3f v1) {
  return sqrt(((v0.x - v1.x) * (v0.x - v1.x)) + ((v0.y - v1.y) * (v0.y - v1.y)) + ((v0.z - v1.z) * (v0.z - v1.z)));
}

int Plyobj::findShortestEdge() {
  int smallestEdgeIndex = 0;
  float smallestLen = vertDist(vertices[edges[0].v0].pos, vertices[edges[0].v1].pos);
  for (int i = 1; i < int(edges.size()); i++) {
    if (edges[i].influencer == -2) continue;
    float edgeLen = vertDist(vertices[edges[i].v0].pos, vertices[edges[i].v1].pos);
    if (edgeLen > smallestLen) {
      smallestEdgeIndex = i;
      smallestLen = edgeLen;
    }
  }
  edges[smallestEdgeIndex].influencer = -2;
  return smallestEdgeIndex;
}

void Plyobj::simplify(int endFaces) {
  while (faceSize > endFaces && faceSize > 4) {
    removeEdge(findShortestEdge());
  }
}

namespace {

uint64_t make_key(int a, int b) {
  if (a > b) {
    std::swap(a, b);
  }
  return (uint64_t(a) << 32) | uint64_t(b);
}

}  // namespace

void Plyobj::buildEdgeList() {
  edges.clear();
  vertsToEdgeIndex.clear();
  unordered_map<int, int> probs;
  int b[] = {1, 2, 0};
  for (size_t i = 0; i < tris.size(); i++) {  // Pass over all the tris
    const auto& t = tris[i];
    for (int j = 0; j < 3; j++) {  // Pass over every edge of the face
      int va = t.v[j];
      int vb = t.v[b[j]];
      int ei = -1;
      {
        uint64_t k = make_key(va, vb);
        auto it = vertsToEdgeIndex.find(k);
        if (it != vertsToEdgeIndex.end()) {
          ei = it->second;
        } else {
          ei = int(edges.size());
          edges.push_back(Edge());
          edgeSize++;
          vertsToEdgeIndex[k] = ei;
        }
      }
      auto& e = edges[ei];
      if (e.v0 < 0) {
        if (va < vb) {
          e.v0 = va;
          e.v1 = vb;
        } else {
          e.v0 = vb;
          e.v1 = va;
        }
      }

      if (va < vb) {
        if (e.f0 != -1) {
          probs[e.f0]++;
          probs[i]++;
        }
        e.f0 = i;
      } else {
        if (e.f1 != -1) {
          probs[e.f1]++;
          probs[i]++;
        }
        e.f1 = i;
      }
    }
  }

  if (probs.size()) {
    int fixed = 0;
    for (auto prob : probs) {
      if (prob.second == 3) {
        auto& t = tris[prob.first];
        swap(t.v[0], t.v[1]);
        fixed++;
      }
    }
    printf("fixed %d face winding problems\n", fixed);
    buildEdgeList();
  }
}

void Plyobj::build(FILE* f, Matrix4f m) {
  fseek(f, 0, SEEK_SET);
  string searchStr = nextLine(f);
  while (int(searchStr.size()) <= 14) searchStr = nextLine(f);
  while (strcmp(searchStr.substr(0, 14).c_str(), "element vertex") != 0) {
    searchStr = nextLine(f);
    while (int(searchStr.size()) <= 14) searchStr = nextLine(f);
  }
  vertSize = atoi(searchStr.substr(15, int(searchStr.size())).c_str());
  vertices.resize(vertSize);
  while (strcmp(searchStr.substr(0, 12).c_str(), "element face") != 0) {
    searchStr = nextLine(f);
    while (int(searchStr.size()) <= 12) searchStr = nextLine(f);
  }
  faceSize = atoi(searchStr.substr(13, int(searchStr.size())).c_str());
  tris.resize(faceSize);
  while (strcmp(nextLine(f).c_str(), "end_header") != 0)
    ;

  for (int i = 0; i < vertSize; i++) {  // Get vertex data
    Vert vertex;
    int ind = 0;
    string sNumber, line = nextLine(f);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        vertex.pos[ind] = stof(sNumber);
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    vertices[i] = vertex;
  }
  for (int i = 0; i < faceSize; i++) {  // Get face data
    auto& t = tris[i];
    int ind = 0;
    string sNumber, line = nextLine(f, 2);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        t.v[ind] = atoi(sNumber.c_str());
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    Vec3f faceNorm = (vertices[t.v[0]].pos - vertices[t.v[2]].pos).Cross((vertices[t.v[1]].pos - vertices[t.v[2]].pos));
    faceNorm.Normalize();
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when Sets
      if (vertices[t.v[j]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[t.v[j]].norm = faceNorm;
      else
        vertices[t.v[j]].norm = (vertices[t.v[j]].norm + faceNorm) / 2;
    }
  }

  buildEdgeList();
  simplify(faceSize - 2);

  obj.begin(GL_TRIANGLES);
  for (int i = 0; i < faceSize; i++) {
    for (int j = 0; j < 3; j++) {
      obj.color(1.0f, 1.0f, 1.0f);
      obj.normal(vertices[tris[i].v[j]].norm);
      // obj.texCoord();
      obj.position((m * vertices[tris[i].v[j]].pos));
    }
  }
  obj.end();
}

void Plyobj::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Plyobj::intersect([[maybe_unused]] Vec3f p0, [[maybe_unused]] Vec3f p1, [[maybe_unused]] Vec3f& intersection) {
  return false;
}
