#include "plyobj.h"

using namespace r3;
using namespace std;

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
  plyFile = f;
  std::vector<Vec3f> verticies;
  std::vector<Vec3f> normals;
  vector<Vec3i> faces;
  fseek(f, 0, SEEK_SET);
  int vertSize = atoi(nextLine(f, 36).c_str());
  verticies.resize(vertSize);
  normals.resize(vertSize);
  int faceSize = atoi(nextLine(f, 70).c_str());
  faces.resize(faceSize);
  while (strcmp(nextLine(f).c_str(), "end_header") != 0)
    ;
  for (int i = 0; i < vertSize; i++) {
    Vec3f vertPos;
    int ind = 0;
    string sNumber, line = nextLine(f);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        vertPos[ind] = stof(sNumber);
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    verticies[i] = vertPos;
    normals[i] = Vec3f(0.0f, 0.0f, 0.0f);
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
    faces[i] = face;
    Vec3f faceNorm = (verticies[face[0]] - verticies[face[2]]).Cross((verticies[face[1]] - verticies[face[2]])).Normalized();
    for (int j = 0; j < 3; j++) {  // Sets normals when not there, and averages when set
      // Not great because it smooths every single vertex regardless of how sharp the difference is
      if (normals[face[j]] == Vec3f(0.0f, 0.0f, 0.0f))
        normals[face[j]] = faceNorm;
      else
        normals[face[j]] = (normals[face[j]] + faceNorm) / 2;
    }
  }
  obj.begin(GL_TRIANGLES);
  obj.color(1.0f, 1.0f, 1.0f);
  for (int i = 0; i < faceSize; i++) {
    Vec3i face = faces[i];
    for (int j = 0; j < 3; j++) {
      obj.normal(normals[face[j]]);
      // obj.texCoord();
      obj.position((m * verticies[face[j]]));
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
