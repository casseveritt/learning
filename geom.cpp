#include "geom.h"

using namespace r3;

namespace {
GLuint dummy_program = 0;
GLuint dummy_buffer = 0;
} // namespace

Geom::Geom() {
  if (dummy_buffer == 0) {
    dummy_program = glCreateProgram();
    glGenBuffers(1, &dummy_buffer);
  }
}

void Geom::begin(GLenum prim) {
  glGenBuffers(1, &b);
  primType = prim;
  verts.clear();
}

void Geom::end() {
  glBindBuffer(GL_ARRAY_BUFFER, b);
  glBufferData(GL_ARRAY_BUFFER, float(sizeof(Vertex) * verts.size()), &verts[0],
               GL_DYNAMIC_DRAW);
}

void Geom::color(float r, float g, float bl) { v.color = Vec3f(r, g, bl); }

void Geom::texCoord(float s, float t) { v.texCoords = Vec2f(s, t); }

void Geom::normal(float x, float y, float z) { v.normal = Vec3f(x, y, z); }

void Geom::normal(Vec3f norm) { v.normal = norm; }

void Geom::position(float x, float y, float z) {
  v.position = Vec3f(x, y, z);
  verts.push_back(v);
}

void Geom::position(Vec3f cords) {
  v.position = cords;
  verts.push_back(v);
}

#define OFFSET_OF(v) reinterpret_cast<void *>(v)

void Geom::draw(const Scene &scene, Prog p) {
  glUseProgram(p.p);
  Matrix4f viewMat = scene.camPose.Inverted().GetMatrix4();
  glBindBuffer(GL_ARRAY_BUFFER, b);
  if (p.col.i >= 0) {
    glVertexAttribPointer(p.col.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(0));
    glEnableVertexAttribArray(p.col.u);
  }
  if (p.norm.i >= 0) {
    glVertexAttribPointer(p.norm.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(sizeof(Vec3f)));
    glEnableVertexAttribArray(p.norm.u);
  }
  if (p.pos.i >= 0) {
    glVertexAttribPointer(p.pos.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(sizeof(Vec3f) * 2));
    glEnableVertexAttribArray(p.pos.u);
  }
  if (p.texCoord.i >= 0) {
    glVertexAttribPointer(p.texCoord.u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          OFFSET_OF(sizeof(Vec3f) * 3));
    glEnableVertexAttribArray(p.texCoord.u);
  }

  Matrix4f modelMat = modelPose.GetMatrix4();
  glUniformMatrix4fv(p.proj.i, 1, GL_FALSE, scene.projMat.GetValue());
  glUniformMatrix4fv(p.view.i, 1, GL_FALSE, viewMat.GetValue());
  glUniformMatrix4fv(p.model.i, 1, GL_FALSE, modelMat.GetValue());
  glUniform3fv(p.lightPos.i, 1, &scene.lightPos.x);
  glUniform3fv(p.lightCol.i, 1, &scene.lightCol.x);
  glUniform3fv(p.matDifCol.i, 1, &matDifCol.x);
  glUniform3fv(p.matSpcCol.i, 1, &matSpcCol.x);
  glUniform1f(p.shiny.i, shiny);
  glUniform3fv(p.camPos.i, 1, &scene.camPos.x);
  glDrawArrays(primType, 0, verts.size());

  if (p.col.i >= 0)
    glDisableVertexAttribArray(p.pos.u);
  if (p.norm.i >= 0)
    glDisableVertexAttribArray(p.norm.u);
  if (p.pos.i >= 0)
    glDisableVertexAttribArray(p.col.u);
  glBindBuffer(GL_ARRAY_BUFFER, dummy_buffer);
  glUseProgram(dummy_program);
}