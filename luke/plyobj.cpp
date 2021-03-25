#include "plyobj.h"

using namespace r3;
using namespace std;

float mag(Vec3f vecIn) {
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

void Plyobj::build(FILE* f, Matrix4f m) {
  std::vector<Vert> vertices;
  vector<Poly> faces;
  fseek(f, 0, SEEK_SET);
  int vertSize = atoi(nextLine(f, 36).c_str());
  vertices.resize(vertSize);
  int faceSize = atoi(nextLine(f, 70).c_str());
  faces.resize(faceSize);
  while (strcmp(nextLine(f).c_str(), "end_header") != 0)
    ;

  for (int i = 0; i < vertSize; i++) {
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
  for (int i = 0; i < faceSize; i++) {
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
    faces[i].points = face;
    Vec3f faceNorm =
        (vertices[face[0]].pos - vertices[face[2]].pos).Cross((vertices[face[1]].pos - vertices[face[2]].pos)).Normalized();
    faces[i].area = mag(faceNorm) / 2;
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when set
      vertices[face[j]].defFaces.push_back(i);
      if (vertices[face[j]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[face[j]].norm = faceNorm;
      else
        vertices[face[j]].norm = (vertices[face[j]].norm + faceNorm) / 2;
    }
  }
  int oldFaceSize = faceSize;
  while (faceSize > (oldFaceSize / 2)) {
    float area = 0.0f;
    int fe0, fe1, p0i, p1i;
    for (int i = 0; i < faceSize; i++) {
      Vert p0, p1;
      Poly f0, f1;
      f0 = faces[i];
      for (int j = 0; j < 3; j++) {
        int f1ind;
        p0 = vertices[f0.points[j]];
        p1 = vertices[f0.points[(j + 1) % 3]];
        Vec3f a0, a1, pm = Vec3f((p0.pos.x + p1.pos.x) / 2, (p0.pos.y + p1.pos.y) / 2, (p0.pos.z + p1.pos.z) / 2);
        a0 = vertices[f0.points[(j + 2) % 3]].pos - pm;
        for (int ii = 0; ii < int(p0.defFaces.size()); ii++) {
          for (int jj = 0; jj < int(p1.defFaces.size()); jj++) {
            if (p0.defFaces[ii] == p1.defFaces[jj] && p0.defFaces[ii] != i) {
              f1ind = p0.defFaces[ii];
              f1 = faces[f1ind];
              for (int k = 0; k < 3; k++) {
                if (f1.points[k] != f0.points[j] && f1.points[k] != f0.points[(j + 1) % 3]) {
                  a1 = vertices[f1.points[k]].pos - pm;
                }
              }
            }
          }
        }
        if (ToDegrees(acos((a0.Dot(a1)) / (mag(a0) * mag(a1)))) > 135) {
          if (area == 0.0f || (f0.area + f1.area) < area) {
            area = (f0.area + f1.area);
            p0i = f0.points[j];
            p1i = f0.points[(j + 1) % 3];
            fe0 = i;
            fe1 = f1ind;
          }
        }
      }
    }
    for (int i = 0; i < int(vertices[p1i].defFaces.size()); i++) {
      int faceIndex = vertices[p1i].defFaces[i];
      if (faceIndex != fe0 && faceIndex != fe1) {
        vertices[p0i].defFaces.push_back(faceIndex);
        for (int j = 0; j < 3; j++) {
          if (faces[faceIndex].points[j] == p1i) {
            faces[faceIndex].points[j] = p0i;
          }
        }
      }
    }
    for (int i = 0; i < 3; i++) {
      int vertIndex = faces[fe0].points[i];
      if (vertIndex != p0i && vertIndex != p1i) {
        for (int j = 0; j < int(vertices[vertIndex].defFaces.size()); j++) {
          if (vertices[vertIndex].defFaces[j] == fe0) {
            vertices[vertIndex].defFaces[j] = vertices[vertIndex].defFaces[int(vertices[vertIndex].defFaces.size())];
            vertices[vertIndex].defFaces.resize(int(vertices[vertIndex].defFaces.size()) - 2);
          }
        }
      }
    }
    for (int i = 0; i < 3; i++) {
      int vertIndex = faces[fe1].points[i];
      if (vertIndex != p0i && vertIndex != p1i) {
        for (int j = 0; j < int(vertices[vertIndex].defFaces.size()); j++) {
          if (vertices[vertIndex].defFaces[j] == fe1) {
            vertices[vertIndex].defFaces[j] = vertices[vertIndex].defFaces[int(vertices[vertIndex].defFaces.size())];
            vertices[vertIndex].defFaces.resize(int(vertices[vertIndex].defFaces.size()) - 2);
          }
        }
      }
    }
    faceSize -= 2;
    printf("%i\n", faceSize);
  }
  obj.begin(GL_TRIANGLES);
  obj.color(1.0f, 1.0f, 1.0f);
  for (int i = 0; i < faceSize; i++) {
    Poly face = faces[i];
    for (int j = 0; j < 3; j++) {
      obj.normal(vertices[face.points[j]].norm);
      // obj.texCoord();
      obj.position((m * vertices[face.points[j]].pos));
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
