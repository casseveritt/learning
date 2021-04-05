
#include <unordered_set>

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

// Would collapsing this edge would split a contiguous volume into
// two separate volumes?
bool isEdgeChoke(const Plyobj& po, size_t edgeIndex) {
  const auto& e = po.edges[edgeIndex];

  vector<size_t> oppv0;
  vector<size_t> oppv1;
  for (size_t i = 0; i < po.edges.size(); i++) {
    if (i == edgeIndex) {
      continue;
    }
    const auto& ee = po.edges[i];
    if (ee.v0 == e.v0) {
      oppv0.push_back(ee.v1);
    } else if (ee.v1 == e.v0) {
      oppv0.push_back(ee.v0);
    }
    if (ee.v0 == e.v1) {
      oppv1.push_back(ee.v1);
    } else if (ee.v1 == e.v1) {
      oppv1.push_back(ee.v0);
    }
  }
  for (auto o0 : oppv0) {
    for (auto o1 : oppv1) {
      if (o0 == o1) {
        IndexTriple i3(e.v0, e.v1, o0);
        if (po.vertsToTriIndex.find(i3) == po.vertsToTriIndex.end()) {
          return true;
        }
      }
    }
  }
  return false;
}

// This builds a set of texture space triangles that correspond to the mesh triangles
void buildUvMap([[maybe_unused]] const Plyobj& po, [[maybe_unused]] vector<Vec2f>& texCoords, [[maybe_unused]] vector<Plyobj::Tri>& texTris) {
  // Start with a pile of triangles that need to be mapped
  // Begin a chart with one triangle and add its available edges to the possible growth
  // direction.  If we try and fail to add a new triangle on an edge, remove it from the
  // list for consideration.
  // Each time we add a new triangle to a chart, remove it from the list that needs
  // mapping.
  // When we run out of edges that we can add triangles to, start a new chart.
  // When we run out of triangles to map, pack the charts into a square, scaling so that
  // the square's coordinates are normalized.
  unordered_set<size_t> available;
  for (size_t i = 0; i < po.tris.size(); i++) {
    available.insert(i); 
  }
  struct ChartTri {
    size_t plyTriIndex;
    Plyobj::Tri t;
  };
  struct Chart {
    vector<Vec2f> texCoords;
    vector<ChartTri> chartTris;
  };

}


}  // namespace

void Plyobj::buildTriMap() {
  vertsToTriIndex.clear();
  for (size_t i = 0; i < tris.size(); i++) {
    const auto& t = tris[i];
    IndexTriple i3(t.v[0], t.v[1], t.v[2]);
    vertsToTriIndex[i3] = int(i);
  }
}

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
        // printf("  before: t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
        t.v[i] = e.v0;
        // printf("  after:  t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
      }
    }
    if (isDegenerate(t)) {
      degenerates++;
      std::swap(t, tris[tris.size() - degenerates]);
      ti--;
    }
  }
  tris.resize(tris.size() - degenerates);
  buildTriMap();
  buildEdgeList();
}

int Plyobj::findEdgeToRemove() {
  typedef pair<size_t, float> IdxLen;
  vector<IdxLen> il;
  for (size_t i = 0; i < edges.size(); i++) {
    float len = (vertices[edges[i].v0].pos - vertices[edges[i].v1].pos).Length();
    il.push_back(make_pair(i, len));
  }
  sort(il.begin(), il.end(), [](const IdxLen& a, const IdxLen& b) { return a.second < b.second; });
  size_t whichEdge = 0;
  while (whichEdge < il.size()) {
    if (isEdgeChoke(*this, il[whichEdge].first)) {
      // printf("choke edge %d skipped\n", int(il[whichEdge].first));
      whichEdge++;
      continue;
    }
    break;
  }
  printf("skipped %d edges, chose edge %d\n", int(whichEdge), int(il[whichEdge].first));
  return il[whichEdge].first;
}

void Plyobj::simplify(size_t endFaces) {
  while (tris.size() > endFaces && tris.size() > 4) {
    removeEdge(findEdgeToRemove());
    printf("%i\n", int(tris.size()));
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
    if (e.f0 < 0 || e.f1 < 0) {
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
    if (recursionLevel > 0) {
      printf("fixed %d face winding problems\n", fixed);
    }
    if (recursionLevel > 5) {
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

  buildTriMap();
  buildEdgeList();

  //simplify(tris.size() / 8);

  vector<Vec2f> texCoords;
  vector<Tri> texTris;
  buildUvMap(*this, texCoords, texTris);

  //*
  obj.begin(GL_TRIANGLES);
  for (size_t i = 0; i < tris.size(); i++) {
    Vec3f faceNorm = (vertices[tris[i].v[0]].pos - vertices[tris[i].v[2]].pos)
                         .Cross((vertices[tris[i].v[1]].pos - vertices[tris[i].v[2]].pos));
    for (int j = 0; j < 3; j++) {
      obj.color(1.0f, 1.0f, 1.0f);
      obj.normal(faceNorm);
      // obj.texCoord();
      obj.position((m * vertices[tris[i].v[j]].pos));
    }
  }
  obj.end();
  /*/
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
  */
  obj.end();
}

void Plyobj::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Plyobj::intersect([[maybe_unused]] Vec3f p0, [[maybe_unused]] Vec3f p1, [[maybe_unused]] Vec3f& intersection) {
  return false;
}
