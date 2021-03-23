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
  fseek(f, 0, SEEK_SET);
  vector<Vec3f> verticies;
  int vertSize = atoi(nextLine(f, 36).c_str());
  int faceSize = atoi(nextLine(f, 70).c_str());
  while (strcmp(nextLine(f).c_str(), "end_header") != 0)
    ;
  for (int i = 0; i < vertSize; i++) {
    Vec3f item;
    int ind = 0;
    string sNumber, line = nextLine(f);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        item[ind] = stof(sNumber);
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    verticies.push_back(item);
  }
  obj.begin(GL_TRIANGLES);
  obj.color(1.0f, 1.0f, 1.0f);
  vector<Vec3f> triVerts;
  triVerts.resize(3);
  for (int i = 0; i < faceSize; i++) {
    int ind = 0;
    string sNumber, line = nextLine(f, 2);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        triVerts[ind] = verticies[atoi(sNumber.c_str())];
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    for (int j=0;j<3;j++) {
      obj.normal((triVerts[(j+1)%3] - triVerts[j]).Cross((triVerts[(j+2)%3] - triVerts[j])));
      // obj.texCoord();
      obj.position((m * triVerts[j]));
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
