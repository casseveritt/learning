#include "plyobj.h"

using namespace r3;
using namespace std;

float lineLen(Vec3f v0, Vec3f v1) {
  return sqrt((v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y) + (v0.z - v1.z) * (v0.z - v1.z));
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
  while (faceSize > endFaces) {
    float edgeLength = 10000.0f;
    int fe0, fe1, pe0, pe1;

    for (int i = 0; i < faceSize; i++) {
      Poly f0 = faces[i], f1;
      for (int j = 0; j < 3; j++) {
        int f1ind;
        Vert p0 = vertices[f0.pInd[j]], p1 = vertices[f0.pInd[(j + 1) % 3]];
        Vec3f a0, a1, pm = Vec3f((p0.pos.x + p1.pos.x) / 2, (p0.pos.y + p1.pos.y) / 2, (p0.pos.z + p1.pos.z) / 2);
        a0 = vertices[f0.pInd[(j + 2) % 3]].pos - pm;

        for (int k = 0; k < faceSize; k++) {  // Finding matching face with edge
          Poly pol = faces[k];
          for (int l = 0; l < 3; l++) {
            int pk0 = pol.pInd[l], pk1 = pol.pInd[(l + 1) % 3];
            if (k != i) {
              if ((pk0 == f0.pInd[j] || pk1 == f0.pInd[j]) && (pk0 == f0.pInd[(j + 1) % 3] || pk1 == f0.pInd[(j + 1) % 3])) {
                f1ind = k;
                f1 = faces[k];
                a1 = vertices[f1.pInd[(k + 2) % 3]].pos - pm;
                break;
              }
            }
          }
        }

        //

        if (ToDegrees(acos((a0.Dot(a1)) / (mag(a0) * mag(a1)))) > 135) {
          if (lineLen(p0.pos, p1.pos) < edgeLength) {
            edgeLength = lineLen(p0.pos, p1.pos);
            fe0 = i;
            fe1 = f1ind;
            pe0 = f0.pInd[j];
            pe1 = f0.pInd[(j + 1) % 3];
          }
        }
      }
    }
    removeEdge(fe0, fe1, pe0, pe1);
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

  printf("Verts: %i\nFaces: %i\n", vertSize, faceSize);

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
    faces[i].pInd = face;
    Vec3f faceNorm =
        (vertices[face[0]].pos - vertices[face[2]].pos).Cross((vertices[face[1]].pos - vertices[face[2]].pos)).Normalized();
    faces[i].area = mag(faceNorm) / 2;
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when set
      if (vertices[face[j]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[face[j]].norm = faceNorm;
      else
        vertices[face[j]].norm = (vertices[face[j]].norm + faceNorm) / 2;
    }
  }

  simplify(faceSize - 50);

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
