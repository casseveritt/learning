#include "glprog.h"
#include "learning.h"
#include "linear.h"
#include "stb.h"
#include <math.h>
#include <stdio.h>
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
bool mode1;
float rad = 2.0;
float theta = 0.0;
Matrix4f projMat;
Vec3f lightPos(0.0f, 1.0f, 0.0f);

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/,
                         int action, int /*mods*/) {
  if (action == GLFW_PRESS) {
    frame = 0;
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case GLFW_KEY_W:
      modelPose.t.z -= 0.1;
      break;
    case GLFW_KEY_A:
      modelPose.t.x -= 0.1;
      break;
    case GLFW_KEY_S:
      modelPose.t.z += 0.1;
      break;
    case GLFW_KEY_D:
      modelPose.t.x += 0.1;
      break;
    case GLFW_KEY_UP:
      rad -= 0.25;
      break;
    case GLFW_KEY_DOWN:
      rad += 0.25;
      break;
    case GLFW_KEY_LEFT:
      theta -= 0.0125f;
      break;
    case GLFW_KEY_RIGHT:
      theta += 0.0125f;
      break;
    case GLFW_KEY_I:
      fovy -= 1.0;
      break;
    case GLFW_KEY_O:
      fovy += 1.0;
      break;
    case GLFW_KEY_Z:
      if (mode1)
        mode1 = false;
      else
        mode1 = true;
      break;
    default:
      break;
    }
  }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action,
                                  int /*mods*/) {
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
  Ui pos, col, norm, proj, view, model, lightPos;

  void set(GLuint program) {
    p = program;
    pos.i = glGetAttribLocation(program, "pos");
    col.i = glGetAttribLocation(program, "col");
    norm.i = glGetAttribLocation(program, "norm");
    proj.i = glGetUniformLocation(program, "proj");
    view.i = glGetUniformLocation(program, "view");
    model.i = glGetUniformLocation(program, "model");
    lightPos.i = glGetUniformLocation(program, "lightPos");
  }
};

#include "cube.h"

int main(void) {

  camPose.t.z = 2.0;
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  make_hints();

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

  GLint version_maj = 0;
  GLint version_min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &version_maj);
  glGetIntegerv(GL_MINOR_VERSION, &version_min);
  printf("GL Version: %d.%d\n", version_maj, version_min);

  GLuint defaultVab;
  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray(defaultVab);

  // programs init begin
  GLuint prog =
      createProgram("progs/Vertex-Shader.vs", "progs/Fragment-Shader.fs");
  glUseProgram(prog);
  Prog program;
  program.set(prog);

  GLuint litProg =
      createProgram("progs/Lit-Vertex.vs", "progs/Lit-Fragment.fs");
  glUseProgram(litProg);
  Prog litProgram;
  litProgram.set(litProg);

  dummy_program = glCreateProgram();
  glGenBuffers(1, &dummy_buffer);
  // programs init end

  // objects init begin
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

  Cube cub;
  cub.begin(-1.0f, 0.0f, -1.0f, 0.75f);

  Object cube;
  makeCube(cube, Matrix4f::Scale(0.375f));
  cube.modelPose.t = Vec3f(-1, 0.375f, -1);

  Sphere sph;
  sph.begin(1.0f, 0.0f, -1.0f, 0.5f);

  Torus tor;
  tor.torObj.modelPose.t = Vec3f(1, 0.25f, 1);
  tor.begin(0.5f, 0.25f);

  Sphere light;
  light.begin(0.0f, 0.0f, 0.0f, 0.0525f);
  // objects init end

  while (!glfwWindowShouldClose(window)) {

    if (drag) {
      Vec2d currPos;
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      diffPos = currPos - prevPos;
      prevPos = currPos;
      theta += diffPos.x * 0.0125f;
    } else {
      diffPos = Vec2d();
    }

    if (mode1) {
      rad += diffPos.x * 0.0125f;
      rad += diffPos.y * 0.0125f;
    } else {
      camPose.t.x = sin(theta) * rad;
      camPose.t.z = cos(theta) * rad;
      camPose.t.y -= diffPos.y * 0.0125f;
    }

    camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -camPose.t,
                       Vec3f(0, 1, 0));

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    float aspect = float(width) / float(height);
    projMat = Perspective(fovy, aspect, 0.1f, 100.0f);
    // Matrix4f viewMat = camPose.Inverted().GetMatrix4();

    grid.draw(program);
    // cub.draw(program);
    cube.draw(litProgram);
    sph.draw(litProgram);
    tor.draw(litProgram);

    light.sphObj.modelPose.t = lightPos;
    light.draw(program);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}
