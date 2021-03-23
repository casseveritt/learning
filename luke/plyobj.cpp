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

void Plyobj::build(FILE* f, Matrix4f m, Vec3f col) {
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
  obj.color(0.0f, 0.0f, 1.0f);
  for (int i = 0; i < faceSize; i++) {
    string sNumber, line = nextLine(f, 2);
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        obj.position((m * verticies[atoi(sNumber.c_str())]));
        sNumber.resize(0);
      } else {
        sNumber.append(1, line[j]);
      }
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
