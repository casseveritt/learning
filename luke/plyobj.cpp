#include "plyobj.h"

using namespace r3;
using namespace std;

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

void Plyobj::removeEdge(int f0, int f1, int p0, int p1) {
  faces[f0] = faces[faceSize - 1];
  faces[f1] = faces[faceSize - 2];
  faceSize -= 2;
  faces.resize(faceSize);
  for (int i = 0; i < faceSize; i++) {
    Poly pol = faces[i];
    for (int j = 0; j < 3; j++) {
      if (pol.pInd[j] == p1) {
        faces[i].pInd[j] = p0;
      }
    }
  }
  printf("FaceSize: %i\n", faceSize);
}

void Plyobj::simplify(int endFaces) {
  while (faceSize > endFaces && faceSize > 4) {
    faceSize -= 2;
  }
}

void Plyobj::findEdges() {
  for (int i = 0; i < faceSize; i++) {  // Pass over all the faces
    Poly f0 = faces[i], f1;
    for (int j = 0; j < 3; j++) {  // Pass over every edge of the face
      int f0p0 = f0.pInd[j], f0p1 = f0.pInd[(j + 1) % 3];
      for (int k = 0; k < faceSize; k++) {  // Checking every face
        Poly pol = faces[k];
        for (int l = 0; l < 3; l++) {  // Checking every edge of face to find match
          int f1p0 = pol.pInd[l], f1p1 = pol.pInd[(l + 1) % 3];
          if (k != i) {
            if (f1p0 == f0p1 && f1p1 == f0p0) {  // Adds edge to vector if a match is found
              Vec3f v0 = vertices[f0p0].pos, v1 = vertices[f0p1].pos;
              Edge e;
              e.len = sqrt((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y) + (v0.z - v1.z) * (v0.z - v1.z));
              e.p0 = f0p0;
              e.p1 = f0p1;
              e.f0 = i;
              e.f1 = k;
              edges.push_back(e);
              edgeSize++;
            }
          }
        }
      }
    }
  }
  printf("Edges: %i\n", edgeSize);
  for (int i = 0; i < edgeSize - 1; i++) {  // Sorts edges by length
    int mind = i;
    for (int j = i; j < edgeSize; j++) {
      if (edges[j].len < edges[mind].len) mind = j;
    }
    Edge e = edges[i];
    edges[i] = edges[mind];
    edges[mind] = e;
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
  faces.resize(faceSize);
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
    Vec3i face;
    int ind = 0;
    string sNumber, line = nextLine(f, 2);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        face[ind] = atoi(sNumber.c_str());
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    faces[i].pInd = face;
    Vec3f faceNorm = (vertices[face[0]].pos - vertices[face[2]].pos).Cross((vertices[face[1]].pos - vertices[face[2]].pos));
    faces[i].area = mag(faceNorm) / 2;
    faceNorm.Normalize();
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when Sets
      if (vertices[face[j]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[face[j]].norm = faceNorm;
      else
        vertices[face[j]].norm = (vertices[face[j]].norm + faceNorm) / 2;
    }
  }

  findEdges();
  simplify(faceSize);

  obj.begin(GL_TRIANGLES);
  obj.color(1.0f, 1.0f, 1.0f);
  for (int i = 0; i < faceSize; i++) {
    for (int j = 0; j < 3; j++) {
      obj.normal(vertices[faces[i].pInd[j]].norm);
      // obj.texCoord();
      obj.position((m * vertices[faces[i].pInd[j]].pos));
    }
  }
  obj.end();
}

void Plyobj::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Plyobj::intersect(Vec3f p0, Vec3f p1, Vec3f& intersection) {
  return false;
}
