#include "glprog.h"
#include "linear.h"
#include "stb.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace r3;

/*
  You need dev packages to build and run this:

  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

float fovy = 60.0f;
Posef camPose;
Posef modelPose;
int frame = 0;
bool drag = false;
Vec2d prevPos;
Vec2d diffPos;
GLuint dummy_program;
GLuint dummy_buffer;

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (action == GLFW_PRESS) {
    frame = 0;
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case GLFW_KEY_W:
      modelPose.t.y += 0.025;
      break;
    case GLFW_KEY_A:
      modelPose.t.x -= 0.025;
      break;
    case GLFW_KEY_S:
      modelPose.t.y -= 0.025;
      break;
    case GLFW_KEY_D:
      modelPose.t.x += 0.025;
      break;
    case GLFW_KEY_UP:
      camPose.t.z -= 0.025;
      break;
    case GLFW_KEY_DOWN:
      camPose.t.z += 0.025;
      break;
    case GLFW_KEY_I:
      fovy -= 1.0;
      break;
    case GLFW_KEY_O:
      fovy += 1.0;
      break;
    default:
      break;
    }
  }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    drag = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &prevPos.x, &prevPos.y);
  }
}

#define OFFSET_OF(v) reinterpret_cast<void *>(v)

class Prog {
public:
  GLuint p;
  union Ui {
    GLuint u;
    GLint i;
  };
  Ui pos, col, proj, view, model;

  void set(GLuint program){
    p = program;
    pos.i = glGetAttribLocation(program, "pos");
    col.i = glGetAttribLocation(program, "col");
    proj.i = glGetUniformLocation(program, "proj");
    view.i = glGetUniformLocation(program, "view");
    model.i = glGetUniformLocation(program, "model");
  }
};

class Object {
public:
  GLuint b;
  GLint pos_loc, col_loc, proj_loc, view_loc, model_loc;
  Posef modelPose;
  GLenum primType;
  struct Vertex {
    Vec3f color;
    Vec3f position;
  };

  Vertex v;
  std::vector<Vertex> verts;

public:
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

  void position(float x, float y, float z) {
    v.position = Vec3f(x, y, z);
    verts.push_back(v);
  }

  void draw(Prog p, Matrix4f projMat, Matrix4f viewMat) {
    glUseProgram(p.p);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glVertexAttribPointer(p.col.u, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), OFFSET_OF(0));
    glVertexAttribPointer(p.pos.u, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), OFFSET_OF(sizeof(Vec3f)));
    glEnableVertexAttribArray(p.col.u);
    glEnableVertexAttribArray(p.pos.u);

    Matrix4f modelMat = modelPose.GetMatrix4();
    glUniformMatrix4fv(p.proj.i, 1, GL_FALSE, projMat.GetValue());
    glUniformMatrix4fv(p.view.i, 1, GL_FALSE, viewMat.GetValue());
    glUniformMatrix4fv(p.model.i, 1, GL_FALSE, modelMat.GetValue());
    glDrawArrays(primType, 0, verts.size());
    glDisableVertexAttribArray(p.pos.u);
    glDisableVertexAttribArray(p.col.u);
    glBindBuffer(GL_ARRAY_BUFFER, dummy_buffer);
    glUseProgram(dummy_program);
  }
};

int main(void) {

  camPose.t.z = 2.0;
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  window = glfwCreateWindow(640, 480, "Learning", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(
  // objects init begin
      window); // This is the point when you can make gl calls
  glfwSwapInterval(1);

  // programs init begin
  GLuint prog =
      createProgram("progs/Vertex-Shader.vs", "progs/Fragment-Shader.fs");
  glUseProgram(prog);
  Prog program;
  program.set(prog);
  dummy_program = glCreateProgram();
  glGenBuffers(1, &dummy_buffer);
  // programs init end

  // objects init begin
  Object tri;
  tri.begin(GL_TRIANGLES);
  tri.color(0.0f, 1.0f, 0.0f);
  tri.position(-0.5f, 0.0f, 0.0f);
  tri.color(1.0f, 0.0f, 0.0f);
  tri.position(0.5f, 0.0f, 0.0f);
  tri.color(1.0f, 0.0f, 0.0f);
  tri.position(-0.5f, 1.0f, 0.0f);
  tri.end();

  Object tri1;
  tri1.begin(GL_TRIANGLES);
  tri1.color(0.0f, 0.0f, 0.0f);
  tri1.position(-0.5f, 0.0f, -1.0f);
  tri1.color(0.0f, 1.0f, 0.0f);
  tri1.position(-1.5f, 0.0f, 0.0f);
  tri1.color(0.0f, 1.0f, 0.0f);
  tri1.position(-0.5f, 1.0f, -1.0f);
  tri1.end();

  Object tri2;
  tri2.begin(GL_TRIANGLES);
  tri2.color(0.0f, 0.0f, 0.0f);
  tri2.position(0.5f, 1.0f, -1.0f);
  tri2.color(0.95f, 0.95f, 0.95f);
  tri2.position(1.5f, 1.0f, 0.0f);
  tri2.color(0.95f, 0.95f, 0.95f);
  tri2.position(0.5f, 0.0f, -1.0f);
  tri2.end();

  Object tri3;
  tri3.begin(GL_TRIANGLES);
  tri3.color(1.0f, 0.0f, 0.0f);
  tri3.position(0.5f, 0.0f, 1.0f);
  tri3.color(0.0f, 0.0f, 1.0f);
  tri3.position(1.5f, 0.0f, 0.0f);
  tri3.color(0.0f, 0.0f, 1.0f);
  tri3.position(0.5f, 1.0f, 1.0f);
  tri3.end();

  Object tri4;
  tri4.begin(GL_TRIANGLES);
  tri4.color(0.95f, 0.95f, 0.95f);
  tri4.position(-0.5f, 1.0f, 1.0f);
  tri4.color(0.0f, 0.0f, 0.0f);
  tri4.position(-1.5f, 1.0f, 0.0f);
  tri4.color(0.0f, 0.0f, 0.0f);
  tri4.position(-0.5f, 0.0f, 1.0f);
  tri4.end();

  Object grid;
  grid.begin(GL_LINES);
  static const int gridsize = 15; // vertical or horizontal size odd
  static const float s = 0.25f;   // spacing of lines
  for (int i = 0; i < gridsize; i++) {
    float shift = (gridsize / 2) * -1 * s + i * s;
    float move = (gridsize / 2) * s;
    grid.color(0.0f, 0.0f, 0.0f);
    grid.position(shift, 0.0f, move);
    grid.color(0.0f, 0.0f, 0.0f);
    grid.position(shift, 0.0f, move * -1);
    grid.color(0.0f, 0.0f, 0.0f);
    grid.position(move, 0.0f, shift);
    grid.color(0.0f, 0.0f, 0.0f);
    grid.position(move * -1, 0.0f, shift);
  }
  grid.end();
  // objects init end

  while (!glfwWindowShouldClose(window)) {

    if (drag) {
      Vec2d currPos;
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      diffPos = currPos - prevPos;
      prevPos = currPos;
      camPose.t.x += diffPos.x * 0.0125f;
      camPose.t.y -= diffPos.y * 0.0125f;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    float aspect = float(width) / float(height);
    Matrix4f projMat = Perspective(fovy, aspect, 0.1f, 100.0f);
    Matrix4f viewMat = camPose.Inverted().GetMatrix4();

    tri.modelPose = modelPose;
    tri.draw(program, projMat, viewMat);
    tri1.draw(program, projMat, viewMat);
    tri2.draw(program, projMat, viewMat);
    tri3.draw(program, projMat, viewMat);
    tri4.draw(program, projMat, viewMat);
    grid.draw(program, projMat, viewMat);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}