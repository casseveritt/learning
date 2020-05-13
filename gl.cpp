#include "cube.h"
#include "geom.h"
#include "glprog.h"
#include "learning.h"
#include "linear.h"
#include "prog.h"
#include "scene.h"
#include "sphere.h"
#include "stb.h"
#include "torus.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using namespace r3;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

float fovy = 60.0f;
Posef modelPose;
int frame = 0;
bool drag = false;
Vec2d prevPos;
Vec2d diffPos;
bool mode1;
float rad = 2.0;
float theta = 0.0;
Scene scene;

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

int main(void) {

  scene.camPose.t.z = 2.0;
  GLFWwindow *window;

  scene.lightPos = Vec3f(0, 1, 0);
  scene.lightCol = Vec3f(1, 1, 1);

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

  // programs init end

  // objects init begin
  Geom grid;
  grid.begin(GL_LINES);
  grid.color(0.90f, 0.90f, 0.90f);
  static const int gridsize = 15; // vertical or horizontal size odd
  static const float s = 0.25f;   // spacing of lines
  for (int i = 0; i < gridsize; i++) {
    float shift = (gridsize / 2) * -1 * s + i * s;
    float move = (gridsize / 2) * s;
    grid.position(shift, 0.0f, move);
    grid.position(shift, 0.0f, move * -1);
    grid.position(move, 0.0f, shift);
    grid.position(move * -1, 0.0f, shift);
  }
  grid.end();

  Geom cube;
  makeCube(cube, Matrix4f::Scale(0.375f));
  cube.modelPose.t = Vec3f(-1, 0.375f, -1);
  cube.matCol = Vec3f(1.0f, 0.0f, 0.0f);

  Sphere sph;
  sph.build(1.0f, 0.0f, -1.0f, 0.5f);
  sph.sphObj.matCol = Vec3f(0.0f, 1.0f, 0.0f);

  Torus tor;
  tor.torObj.modelPose.t = Vec3f(1, 0.25f, 1);
  tor.build(0.5f, 0.25f);
  tor.torObj.matCol = Vec3f(0.0f, 0.0f, 1.0f);

  Sphere light;
  light.build(0.0f, 0.0f, 0.0f, 0.0525f);
  // light
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
      scene.camPose.t.x = sin(theta) * rad;
      scene.camPose.t.z = cos(theta) * rad;
      scene.camPose.t.y -= diffPos.y * 0.0125f;
    }

    scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t,
                             Vec3f(0, 1, 0));

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.25f, 0.25f, 0.25f, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    float aspect = float(width) / float(height);
    scene.projMat = Perspective(fovy, aspect, 0.1f, 100.0f);
    // Matrix4f viewMat = scene.camPose.Inverted().GetMatrix4();

    grid.draw(scene, program);
    // cub.draw(program);
    cube.draw(scene, litProgram);
    sph.draw(scene, litProgram);
    tor.draw(scene, litProgram);

    light.sphObj.modelPose.t = scene.lightPos;
    light.draw(scene, program);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}
