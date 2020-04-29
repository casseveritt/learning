#include "glprog.h"
#include "linear.h"
#include "stb.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>

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

static void printMatrix(const Matrix4f &m) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%.2f ", m.el(i, j));
    }
    printf("\n");
  }
}

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

class Object {
public:
  GLuint p, b;
  GLint pos_loc, col_loc, proj_loc, view_loc, model_loc;
  int numv;
  Posef modelPose;
  GLenum primType;

public:
  GLsizeiptr sz(int ncomp, int size) { return ncomp * size * numv; }

  void init(GLuint program, int numVerts, Vec3f *pos, Vec3f *col) {
    p = program;
    pos_loc = glGetAttribLocation(program, "pos");
    col_loc = glGetAttribLocation(program, "col");
    proj_loc = glGetUniformLocation(program, "proj");
    view_loc = glGetUniformLocation(program, "view");
    model_loc = glGetUniformLocation(program, "model");
    numv = numVerts;
    glGenBuffers(1, &b);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, sz(6, sizeof(float)), nullptr,
                 GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sz(3, sizeof(float)), pos);
    glBufferSubData(GL_ARRAY_BUFFER, sz(3, sizeof(float)), sz(3, sizeof(float)),
                    col);
    primType = GL_TRIANGLES;
  }

  void render(int numVerts, Matrix4f projMat, Matrix4f viewMat) {
    glUseProgram(p);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glVertexAttribPointer(static_cast<GLuint>(pos_loc), 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), OFFSET_OF(0));
    glVertexAttribPointer(static_cast<GLuint>(col_loc), 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), OFFSET_OF(sz(3, sizeof(float))));
    glEnableVertexAttribArray(static_cast<GLuint>(pos_loc));
    glEnableVertexAttribArray(static_cast<GLuint>(col_loc));

    Matrix4f modelMat = modelPose.GetMatrix4();
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projMat.GetValue());
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, viewMat.GetValue());
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, modelMat.GetValue());
    glDrawArrays(primType, 0, numv);
    glDisableVertexAttribArray(static_cast<GLuint>(pos_loc));
    glDisableVertexAttribArray(static_cast<GLuint>(col_loc));
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
      window); // This is the point when you can make gl calls
  glfwSwapInterval(1);

  // program initialization begin
  GLuint program =
      createProgram("progs/Vertex-Shader.vs", "progs/Fragment-Shader.fs");
  GLint pos_loc = glGetAttribLocation(program, "pos");
  GLint col_loc = glGetAttribLocation(program, "col");

  glUseProgram(program);
  GLint proj_loc = glGetUniformLocation(program, "proj");
  GLint view_loc = glGetUniformLocation(program, "view");
  GLint model_loc = glGetUniformLocation(program, "model");
  // program initialization end

  dummy_program = glCreateProgram();
  glGenBuffers(1, &dummy_buffer);

  // triangle initialization begin
  Object tri;
  Vec3f col[] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
  {
    Vec3f pos[] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};

    tri.init(program, 3, pos, col);
  }
  // triangle initialization end

  // grid init begin
  Object grid;
  static const int gridsize = 7; // vertical or horizontal size odd
  static const int numGridVerts = gridsize * 4;
  {
    Vec3f gridPos[numGridVerts];
    Vec3f gridCol[numGridVerts];
    for (int j = 0; j < gridsize; j++) {
      float frac = j / (gridsize - 1.0f);
      float v = -1 * (1 - frac) + 1 * frac;
      gridPos[(j * 4) + 0] = Vec3f(v, 0, -1);
      gridPos[(j * 4) + 1] = Vec3f(v, 0, 1);
      gridPos[(j * 4) + 2] = Vec3f(1, 0, v);
      gridPos[(j * 4) + 3] = Vec3f(-1, 0, v);
    }
    for (int j = 0; j < numGridVerts; j++) {
      gridCol[j] = Vec3f(1.0f, 0.0f, 0.0f);
    }

    grid.init(program, numGridVerts, gridPos, gridCol);
    grid.primType = GL_LINES;
  }
  // grid init end

  float x = 2.0;
  float y = 2.0;
  bool dir = true;
  bool lorr = true;
  int vertcol[] = {0, 1, 2};
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
    tri.render(3, projMat, viewMat);
    grid.render(2, projMat, viewMat);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}