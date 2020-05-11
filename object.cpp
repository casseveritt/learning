#include "object.h"

using namespace r3;

  void begin(GLenum prim) {
    glGenBuffers(1, &b);
    primType = prim;
    verts.clear();
  }

  void end() {
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, float(sizeof(Vertex) * verts.size()),
                 &verts[0], GL_DYNAMIC_DRAW);
  }

  void color(float r, float g, float bl) { v.color = Vec3f(r, g, bl); }

  void normal(float x, float y, float z) { v.normal = Vec3f(x, y, z); }

  void normal(Vec3f norm) { v.normal = norm; }

  void position(float x, float y, float z) {
    v.position = Vec3f(x, y, z);
    verts.push_back(v);
  }

  void position( Vec3f cords ) {
    v.position = cords;
    verts.push_back(v);
  }

  void draw(Prog p) {
    glUseProgram(p.p);
    Matrix4f viewMat = camPose.Inverted().GetMatrix4();
    glBindBuffer(GL_ARRAY_BUFFER, b);
    if(p.col.i >= 0){
      glVertexAttribPointer(p.col.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(0));
      glEnableVertexAttribArray(p.col.u);
    }if(p.norm.i >= 0){
      glVertexAttribPointer(p.norm.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(sizeof(Vec3f)));
      glEnableVertexAttribArray(p.norm.u);
    }if(p.pos.i >= 0){
      glVertexAttribPointer(p.pos.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(sizeof(Vec3f)*2));
      glEnableVertexAttribArray(p.pos.u);
    }

    Matrix4f modelMat = modelPose.GetMatrix4();
    glUniformMatrix4fv(p.proj.i, 1, GL_FALSE, projMat.GetValue());
    glUniformMatrix4fv(p.view.i, 1, GL_FALSE, viewMat.GetValue());
    glUniformMatrix4fv(p.model.i, 1, GL_FALSE, modelMat.GetValue());
    glUniform3fv(p.lightPos.i, 1, &lightPos.x);
    glDrawArrays(primType, 0, verts.size());

    if(p.col.i >= 0)  glDisableVertexAttribArray(p.pos.u);
    if(p.norm.i >= 0) glDisableVertexAttribArray(p.norm.u);
    if(p.pos.i >= 0)  glDisableVertexAttribArray(p.col.u);
    glBindBuffer(GL_ARRAY_BUFFER, dummy_buffer);
    glUseProgram(dummy_program);
  }