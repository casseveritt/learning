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

bool isDegenerate(const Plyobj::Tri& t) {
  return t.v[0] == t.v[1] || t.v[0] == t.v[2] || t.v[1] == t.v[2];
}

}  // namespace

void Plyobj::removeEdge(size_t eInt) {
  Edge e = edges[eInt];
  // change every Tri that referred
  // to e.v1 to now refer to e.v0
  int degenerates = 0;
  printf("replacing %d with %d\n", e.v1, e.v0);
  for (size_t ti = 0; ti < tris.size() - degenerates; ti++) {
    auto& t = tris[ti];
    for (int i = 0; i < 3; i++) {
      if (t.v[i] == e.v1) {
        printf("  before: t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
        t.v[i] = e.v0;
        printf("  after:  t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
      }
    }
    if (isDegenerate(t)) {
      degenerates++;
      std::swap(t, tris[tris.size() - degenerates]);
      ti--;
    }
  }
  tris.resize(tris.size() - degenerates);
  edges.clear();
  vertsToEdgeIndex.clear();
}

int Plyobj::findEdgeToRemove() {
  typedef pair<size_t,float> IdxLen;
  vector<IdxLen> il;
  for (size_t i = 0; i < edges.size(); i++) {
    float len = (vertices[edges[i].v0].pos-vertices[edges[i].v1].pos).Length();
    il.push_back(make_pair(i, len));
  }
  sort(il.begin(), il.end(), [](const IdxLen& a, const IdxLen& b) { return a.second < b.second; });
  return il[0].first;
}

void Plyobj::simplify(size_t endFaces) {
  while (tris.size() > endFaces && tris.size() > 4) {
    removeEdge(findEdgeToRemove());
    printf("%i\n", int(tris.size()));
    buildEdgeList();
  }
}

void Plyobj::buildEdgeList(int recursionLevel) {
  vertsToEdgeIndex.clear();
  edges.clear();
  vertsToEdgeIndex.clear();
  unordered_map<int, int> probs;
  int b[] = {1, 2, 0};
  for (size_t i = 0; i < tris.size(); i++) {  // Pass over all the tris
    const auto& t = tris[i];
    for (int j = 0; j < 3; j++) {  // Pass over every edge of the face
      int va = t.v[j];
      int vb = t.v[b[j]];
      const auto k = IndexPair(va, vb);
      int ei = -1;
      {
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
        e.v0 = k.min();
        e.v1 = k.max();
      }

      auto& f = (va < vb) ? e.f0 : e.f1;
      if (f != -1) {
        probs[f]++;
        probs[i]++;
      } else {
        f = i;
      }

    }
  }

  int dangling = 0;
  for (size_t i = 0; i < edges.size(); i++) {
    const auto& e = edges[i];
    if ( e.f0 < 0 || e.f1 < 0 ) {
      printf("dangling edge %d: t = {%d, %d}, v = {%d, %d}\n", int(i), e.f0, e.f1, e.v0, e.v1);
      dangling++;
    }
  }
  if (dangling > 0) {
    printf("found %d dangling edges\n", dangling);
  }

  if (probs.size()) {
    int fixed = 0;
    if (recursionLevel == 1) {
      printf("found %d problems\n", int(probs.size()));
    }
    for (auto prob : probs) {
      if (recursionLevel == 1) {
        printf("  tri %d had %d problems\n", int(prob.first), prob.second);
        auto& t = tris[prob.first];
        printf("     v = {%d, %d, %d}\n", t.v[0], t.v[1], t.v[2]);
      }
      if (prob.second == 3) {
        auto& t = tris[prob.first];
        swap(t.v[0], t.v[1]);
        fixed++;
      }
    }
    if ( recursionLevel > 0 ) {
      printf("fixed %d face winding problems\n", fixed);
    }
    if ( recursionLevel > 5 ) {
      printf("probable infinite recursion - exiting\n");
      exit(1);
    }
    buildEdgeList(recursionLevel + 1);
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
  int sz = atoi(searchStr.substr(15, int(searchStr.size())).c_str());
  vertices.resize(sz);
  while (strcmp(searchStr.substr(0, 12).c_str(), "element face") != 0) {
    searchStr = nextLine(f);
    while (int(searchStr.size()) <= 12) searchStr = nextLine(f);
  }
  sz = atoi(searchStr.substr(13, int(searchStr.size())).c_str());
  tris.resize(sz);
  while (strcmp(nextLine(f).c_str(), "end_header") != 0)
    ;

  for (size_t i = 0; i < vertices.size(); i++) {  // Get vertex data
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
  for (size_t i = 0; i < tris.size(); i++) {  // Get face data
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

  simplify(tris.size() - 200);

  /*
  obj.begin(GL_TRIANGLES);
  for (size_t i = 0; i < tris.size(); i++) {
    for (int j = 0; j < 3; j++) {
      obj.color(1.0f, 1.0f, 1.0f);
      obj.normal(vertices[tris[i].v[j]].norm);
      // obj.texCoord();
      obj.position((m * vertices[tris[i].v[j]].pos));
    }
  }
  obj.end();
  */
  obj.begin(GL_LINES);
  obj.color(1.0f, 1.0f, 1.0f);
  for (size_t i = 0; i < edges.size(); i++) {
    const auto& e = edges[i];
    for (int j = 0; j < 2; j++) {
      const auto& v = vertices[j == 0 ? e.v0 : e.v1];
      obj.normal(v.norm);
      obj.position((m * v.pos));
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
