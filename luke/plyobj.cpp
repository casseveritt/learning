#include "plyobj.h"

using namespace r3;
using namespace std;

int edgeSize;

uint64_t mapInd(int v0, int v1);
uint64_t mapInd(int v0, int v1) {
  uint64_t e0 = v0, e1 = v1;
  uint64_t e = e0 | (e1 << 32);
  return e;
}

float Plyobj::mag(Vec3f vecIn) {
  return sqrt((vecIn.x * vecIn.x) + (vecIn.y * vecIn.y) + (vecIn.z * vecIn.z));
}

string Plyobj::nextLine(FILE* f, int offset) {
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
      }
    }
  }
}

void Plyobj::simplify(int endFaces) {
  while (faceSize > endFaces && faceSize > 4) {
    faceSize -= 2;
  }
}

static uint64_t make_key(int a, int b) {
  if (a > b) {
    std::swap(a, b);
  }
  return (uint64_t(a) << 32) | uint64_t(b);
}

void Plyobj::buildEdgeList() {
  edges.clear();
  vertsToEdgeIndex.clear();
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
          tris[e.f0].probs++;
          tris[i].probs++;
        }
        e.f0 = i;
      } else {
        if (e.f1 != -1) {
          tris[e.f1].probs++;
          tris[i].probs++;
        }
        e.f1 = i;
      }
    }
  }
  int probs = 0;
  for (size_t i = 0; i < tris.size(); i++) {
    if (tris[i].probs == 1) {
      probs++;
      tris[i].probs = 0;
    }
    if (tris[i].probs == 3) {
      probs += 3;
      swap(tris[i].v[0], tris[i].v[1]);
      tris[i].probs = 0;
    }
  }
  if (probs != 0) buildEdgeList();
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
    t.area = mag(faceNorm) / 2;
    faceNorm.Normalize();
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when Sets
      if (vertices[t.v[j]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[t.v[j]].norm = faceNorm;
      else
        vertices[t.v[j]].norm = (vertices[t.v[j]].norm + faceNorm) / 2;
    }
  }

  buildEdgeList();
  // printf("Faces: %i\n", int(tris.size()));
  // removeEdge(edgeSize-1);
  // printf("Faces: %i\n", int(tris.size()));
  // simplify(faceSize);

  obj.begin(GL_TRIANGLES);
  obj.color(1.0f, 1.0f, 1.0f);
  for (int i = 0; i < faceSize; i++) {
    for (int j = 0; j < 3; j++) {
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
