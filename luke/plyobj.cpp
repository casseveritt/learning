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
        if (po.vertsToTriIndex.count(i3) == 0) {
          return true;
        }
      }
    }
  }
  return false;
}

/*
// Sets pointers of a BV in the BVH to a split version of itself, and once the BVs
// contain the same amount or less than the specified number or tris.
void split_bv(/Bounding Volume Structure/) {
   if (tris.size()<threshold) return;
   Vec4f plane = choose_dividing_plane();
   child[0].tris.clear();
   child[1].tris.clear();
   for(auto tri : tris) {
       auto center = tri.center();
       child_index =  plane.distance(center) < 0.0f ? 0 : 1;
       child[child_index].tris.push_back( tri );
    }
    tris.clear();
    child[0].split_bv();
    child[1].split_bv();
 }
 */

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

  {  // Deletes edges necessary and fixes map and vector.

    auto rm = [=](int v0, int v1) {
      IndexPair eip(v0, v1);
      int rmEdgeIndex = vertsToEdgeIndex[eip];
      vertsToEdgeIndex.erase(eip);
      auto& ee = edges.back();
      vertsToEdgeIndex[ee.getIndexPair()] = rmEdgeIndex;
      edges[rmEdgeIndex] = ee;
      edges.pop_back();
    };

    rm(e.v0, e.v1);

    auto get_other_vert = [=](int f) -> int {
      auto& t = tris[f];
      for (int i = 0; i < 3; i++) {
        if (t.v[i] != e.v0 && t.v[i] != e.v1) {
          return t.v[i];
        }
      }
      return ~0;  // this should not happen
    };

    rm(get_other_vert(e.f0), e.v1);
    rm(get_other_vert(e.f1), e.v1);
  }

  // change every Tri that referred
  // to e.v1 to now refer to e.v0
  int degenerates = 0;
  printf("replacing %d with %d\n", e.v1, e.v0);
  for (size_t ti = 0; ti < tris.size() - degenerates; ti++) {
    auto& t = tris[ti];
    IndexTriple i3 = t.getIndexTriple();
    for (int i = 0; i < 3; i++) {
      if (t.v[i] == e.v1) {
        // printf("  before: t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
        t.v[i] = e.v0;
        // printf("  after:  t = %d, v = {%d, %d, %d}\n", int(ti), t.v[0], t.v[1], t.v[2]);
      }
    }
    IndexTriple i3new = t.getIndexTriple();
    if (i3 != i3new) {
      vertsToTriIndex.erase(i3);
      if (isDegenerate(t)) {
        degenerates++;
        auto end = tris.size() - degenerates;
        if (ti != end) {
          auto i3end = tris[end].getIndexTriple();
          std::swap(t, tris[end]);
          vertsToTriIndex[i3end] = ti;
          ti--;
        }
      } else {
        vertsToTriIndex[i3new] = int(ti);
      }
    }
  }
  tris.resize(tris.size() - degenerates);

  // debug check
  int dangling = 0;
  for (size_t i = 0; i < edges.size(); i++) {
    const auto& edg = edges[i];
    if (edg.f0 < 0 || edg.f1 < 0) {
      printf("dangling edge %d: t = {%d, %d}, v = {%d, %d}\n", int(i), edg.f0, edg.f1, edg.v0, edg.v1);
      dangling++;
    }
  }
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
  printf("skipped %d edges, chose edge %d, f# = %d, mf# = %d\n", int(whichEdge), int(il[whichEdge].first), int(tris.size()),
         int(vertsToTriIndex.size()));
  return il[whichEdge].first;
}

void Plyobj::simplify(size_t endTris) {
  while (tris.size() > endTris && tris.size() > 4) {
    removeEdge(findEdgeToRemove());
    for (size_t i = 0; i < tris.size(); i++) {
      auto& t = tris[i];
      if (vertsToTriIndex.count(t.getIndexTriple()) == 0) {
        printf("f %d = {%d, %d, %d} not in map!\n", int(i), t.v[0], t.v[1], t.v[2]);
      }
    }
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
    for (int j = 0; j < 3; j++) {  // Pass over every edge of the tri
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

      if (e.f0 > -1 && e.f1 > -1) {
        Vec3f f0norm = (vertices[tris[e.f0].v[0]].pos - vertices[tris[e.f0].v[2]].pos)
                           .Cross((vertices[tris[e.f0].v[1]].pos - vertices[tris[e.f0].v[2]].pos));
        Vec3f f1norm = (vertices[tris[e.f0].v[0]].pos - vertices[tris[e.f0].v[2]].pos)
                           .Cross((vertices[tris[e.f0].v[1]].pos - vertices[tris[e.f0].v[2]].pos));
        float f0mag = sqrt((f0norm.x * f0norm.x) + (f0norm.y * f0norm.y) + (f0norm.z * f0norm.z));
        float f1mag = sqrt((f1norm.x * f1norm.x) + (f1norm.y * f1norm.y) + (f1norm.z * f1norm.z));
        e.angle = ToDegrees(acos(f0norm.Dot(f1norm) / (f0mag * f1mag)));
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
      printf("fixed %d tri winding problems\n", fixed);
    }
    if (recursionLevel > 5) {
      printf("probable infinite recursion - exiting\n");
      exit(1);
    }
    printf("Rebuilding...\n");
    buildEdgeList(recursionLevel + 1);
  }
}

void Plyobj::appendTri(vector<int> vertInds) {
  int triVertsSize = int(vertInds.size());
  switch (triVertsSize) {
    case 3:

    default:
      break;
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
  triSize = atoi(searchStr.substr(13, int(searchStr.size())).c_str());
  // tris.resize(sz);
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
    vertex.pos = m * vertex.pos;
    if (i == 0) {
      boundingMax = boundingMin = vertex.pos;
    } else {
      if (vertex.pos.x > boundingMax.x) boundingMax.x = vertex.pos.x;
      if (vertex.pos.x < boundingMin.x) boundingMin.x = vertex.pos.x;

      if (vertex.pos.y > boundingMax.y) boundingMax.y = vertex.pos.y;
      if (vertex.pos.y < boundingMin.y) boundingMin.y = vertex.pos.y;

      if (vertex.pos.z > boundingMax.z) boundingMax.z = vertex.pos.z;
      if (vertex.pos.z < boundingMin.z) boundingMin.z = vertex.pos.z;
    }
    vertices[i] = vertex;
  }
  for (int i = 0; i < int(vertices.size()); i++) vertices[i].pos.y -= boundingMin.y;
  for (int i = 0; i < triSize; i++) {  // Get tri data
    Tri t;
    string sNumber, line = nextLine(f, 2);
    int ind = 0;
    for (size_t j = 0; j < line.size(); j++) {
      if (line[j] == ' ') {
        t.v[ind] = atoi(sNumber.c_str());
        sNumber.resize(0);
        ind++;
      } else {
        sNumber.append(1, line[j]);
      }
    }
    // printf("\n");
    tris.push_back(t);
    Vec3f triNorm = (vertices[t.v[0]].pos - vertices[t.v[2]].pos).Cross((vertices[t.v[1]].pos - vertices[t.v[2]].pos));
    triNorm.Normalize();
    for (int k = 0; k < 3; k++) {  // Sets normals when not there, and averages when Sets
      if (vertices[t.v[k]].norm == Vec3f(0.0f, 0.0f, 0.0f))
        vertices[t.v[k]].norm = triNorm;
      else
        vertices[t.v[k]].norm = (vertices[t.v[k]].norm + triNorm) / 2;
    }
  }

  // buildTriMap();
  // buildEdgeList();

  // simplify(tris.size() - 800);

  //* Polygonal
  obj.begin(GL_TRIANGLES);
  for (size_t i = 0; i < tris.size(); i++) {
    Vec3f triNorm = (vertices[tris[i].v[0]].pos - vertices[tris[i].v[2]].pos)
                        .Cross((vertices[tris[i].v[1]].pos - vertices[tris[i].v[2]].pos));
    for (int j = 0; j < 3; j++) {
      obj.color(1.0f, 1.0f, 1.0f);
      obj.normal(triNorm);
      // obj.texCoord();
      obj.position((vertices[tris[i].v[j]].pos));
    }
  }
  obj.end();
  /*/ Wireframe
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
  */
}

void Plyobj::draw(const Scene& scene, Prog p) {
  obj.draw(scene, p);
}

bool Plyobj::triInt(Vec3f p0, Vec3f p1, Tri tr, Vec3f& intPoint) {
  const float EPSILON = 0.0000001;
  Vec3f v0 = vertices[tr.v[0]].pos;
  Vec3f v1 = vertices[tr.v[1]].pos;
  Vec3f v2 = vertices[tr.v[2]].pos;
  Vec3f edge1, edge2, h, s, q;
  float a, f, u, v;
  edge1 = v1 - v0;
  edge2 = v2 - v0;
  h = p1.Cross(edge2);
  a = edge1.Dot(h);
  // This ray is parallel to this triangle.
  if (a > -EPSILON && a < EPSILON) return false;
  f = 1.0 / a;
  s = p0 - v0;
  u = f * s.Dot(h);
  if (u < 0.0 || u > 1.0) return false;
  q = s.Cross(edge1);
  v = f * p1.Dot(q);
  // At this stage we can compute t to find out where the intersection point is on the line.
  if (v < 0.0 || u + v > 1.0) return false;
  float t = f * edge2.Dot(q);
  if (t > EPSILON) {  // ray intersection
    intPoint = p0 + p1 * t;
    return true;
  }  // This means that there is a line intersection but not a ray intersection.
  else
    return false;
}

bool Plyobj::intersect([[maybe_unused]] Vec3f p0, [[maybe_unused]] Vec3f p1, [[maybe_unused]] Vec3f& intersection) {
  Vec3f r0 = p0 - obj.modelPose.t;
  Vec3f r1 = p1 - obj.modelPose.t;
  vector<Vec3f> intPoints;
  bool inBounds = false;

  Linef line(p0, p1);
  vector<Planef> planes;
  planes.push_back(Planef(Vec3f(1.0f, 0.0f, 0.0f), boundingMax.x));
  planes.push_back(Planef(Vec3f(-1.0f, 0.0f, 0.0f), boundingMin.x));
  planes.push_back(Planef(Vec3f(0.0f, 1.0f, 0.0f), boundingMax.y));
  planes.push_back(Planef(Vec3f(0.0f, -1.0f, 0.0f), boundingMin.y));
  planes.push_back(Planef(Vec3f(0.0f, 0.0f, 1.0f), boundingMax.z));
  planes.push_back(Planef(Vec3f(0.0f, 0.0f, -1.0f), boundingMin.z));

  for (Planef p : planes) {
    Vec3f pip;  // PlaneIntPoint
    if (p.Intersect(line, pip)) {
      if (pip.x <= boundingMax.x && pip.y <= boundingMax.y && pip.z <= boundingMax.z) {
        if (pip.x >= boundingMin.x && pip.y >= boundingMin.y && pip.z >= boundingMin.z) {
          inBounds = true;
        }
      }
    }
  }

  if (inBounds) {
    printf("In bounds\n");
    for (int i = 0; i < int(tris.size()); i++) {
      auto& t = tris[i];
      Vec3f hp;
      if (triInt(r0, r1, t, hp)) intPoints.push_back(hp);
    }
    if (!intPoints.empty()) {
      Vec3f closestInt = intPoints[0];
      float minLen = (r0 - closestInt).Length();
      for (int i = 1; i < int(intPoints.size()); i++) {
        float len = (r0 - intPoints[i]).Length();
        if (len < minLen) {
          closestInt = intPoints[i];
          minLen = len;
        }
      }
      intersection = closestInt + obj.modelPose.t;
      return true;
    }
    return false;
  }
}
