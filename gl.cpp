#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "cube.h"
#include "geom.h"
#include "learning.h"
#include "linear.h"
#include "prog.h"
#include "scene.h"
#include "sphere.h"
#include "stb.h"
#include "torus.h"

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
float rad = 2.5;
float theta = 0.0;
Scene scene;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
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

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    drag = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &prevPos.x, &prevPos.y);
  }
}

static GLuint load_image(const char* imgName) {
  int w, h, n;

  const char* im = "imgs/";
  char* imgLocation = new char[strlen(im) + strlen(imgName) + 1];
  strcpy(imgLocation, im);
  strcat(imgLocation, imgName);

  GLuint out;
  unsigned char* img;

  img = image_load(imgLocation, &w, &h, &n);

  delete[] imgLocation;

  uint32_t* imgi = new uint32_t[w * h];

  for (int i = 0; i < w * h; i++) {
    imgi[i] = uint32_t(img[i * 3 + 0]) << 0 | uint32_t(img[i * 3 + 1]) << 8 | uint32_t(img[i * 3 + 2]) << 16;
  }

  glGenTextures(GLsizei(n), &out);
  glBindTexture(GL_TEXTURE_2D, out);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgi);
  glGenerateMipmap(GL_TEXTURE_2D);
  delete[] imgi;
  image_free(img);
  return out;
}

static Vec3f evalBezier(const std::vector<Vec3f>& p, float t) {
  float t2 = t * t;
  float t3 = t * t * t;
  float w[4] = {-1 * t3 + 3 * t2 - 3 * t + 1, 3 * t3 - 6 * t2 + 3 * t, -3 * t3 + 3 * t2, t3};
  return p[0] * w[0] + p[1] * w[1] + p[2] * w[2] + p[3] * w[3];
}

static Vec3f evalDeCast(const std::vector<Vec3f>& p, float t) {
  if (p.size() == 1) {
    return p[0];
  }

  std::vector<Vec3f> k;
  for (size_t j = 0; j < p.size() - 1; j++) {
    k.push_back(p[j] + ((p[j + 1] - p[j]) * t));
  }
  return evalDeCast(k, t);
}

int main(void) {
  scene.camPose.t.z = 2.0;
  GLFWwindow* window;

  scene.lightPos = Vec3f(0, 1, 0);
  scene.lightCol = Vec3f(1, 1, 1);

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  make_hints();
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  window = glfwCreateWindow(1280, 960, "Learning", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);  // This is the point when you can make gl calls
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
  Prog program("Unlit");
  Prog litProgram("Lit");
  Prog texProgram("Tex");
  Prog coordProgram("Coord");
  Prog litTexProgram("LitTex");
  // programs init end

  // Texture begin
  GLuint check, brick, stone, wood;

  check = load_image("check.png");
  brick = load_image("bricks.png");
  stone = load_image("stone.png");
  wood = load_image("wood.png");
  // Texture end

  // objects init begin
  Geom grid;
  grid.begin(GL_LINES);
  grid.color(0.90f, 0.90f, 0.90f);
  static const int gridsize = 17;  // vertical or horizontal size odd
  static const float s = 0.25f;    // spacing of lines
  for (int i = 0; i < gridsize; i++) {
    float shift = (gridsize / 2) * -1 * s + i * s;
    float move = (gridsize / 2) * s;
    grid.position(shift, 0.0f, move);
    grid.position(shift, 0.0f, move * -1);
    grid.position(move, 0.0f, shift);
    grid.position(move * -1, 0.0f, shift);
  }
  grid.end();

  float sqrDime = float(gridsize / 2) * s;
  float sqrSize = float((gridsize - 1) / 2);

  Geom square;
  square.begin(GL_TRIANGLE_STRIP);
  square.normal(0, 1, 0);
  square.color(0.0f, 0.0f, 0.0f);
  square.texCoord(0.0f, 0.0f);
  square.position(-sqrDime, 0.0f, -sqrDime);
  square.texCoord(0.0f, sqrSize);
  square.position(-sqrDime, 0.0f, sqrDime);
  square.texCoord(sqrSize, 0.0f);
  square.position(sqrDime, 0.0f, -sqrDime);
  square.texCoord(sqrSize, sqrSize);
  square.position(sqrDime, 0.0f, sqrDime);
  square.end();
  square.tex = check;

  square.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
  square.shiny = 40.0f;

  Cube cube;
  cube.build(Matrix4f::Scale(0.375f));
  cube.obj.modelPose.t = Vec3f(-1, 0.375f, -1);
  cube.obj.matDifCol = Vec3f(0.7f, 0.0f, 0.0f);
  cube.obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
  cube.obj.shiny = 7.5f;
  cube.obj.tex = brick;

  Sphere sph;
  sph.build(0.5f);
  sph.obj.modelPose.t = Vec3f(1.0, 0.5, -1.0);
  sph.obj.matDifCol = Vec3f(0.0f, 0.3f, 0.0f);
  sph.obj.matSpcCol = Vec3f(0.2f, 1.0f, 0.2f);
  sph.obj.shiny = 25.0f;
  sph.obj.tex = stone;

  Torus tor;
  tor.build(0.5f, 0.25f);
  tor.obj.modelPose.t = Vec3f(1, 0.25f, 1);
  tor.obj.matDifCol = Vec3f(0.0f, 0.0f, 0.5f);
  tor.obj.matSpcCol = Vec3f(0.3f, 0.3f, 1.0f);
  tor.obj.shiny = 15.0f;
  tor.obj.tex = wood;

  Sphere light;  // light
  light.build(0.03125f);

  std::vector<Vec3f> points;
  points.push_back(Vec3f(-1.0f, 0.0f, 1.0f));
  points.push_back(Vec3f(-1.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 0.0f, 1.0f));
  Geom hull;
  hull.begin(GL_LINE_STRIP);
  hull.color(0.0f, 0.0f, 0.0f);
  for (auto v : points) {
    hull.position(v);
  }
  hull.end();

  Geom curve;
  curve.begin(GL_LINE_STRIP);
  curve.color(1.0f, 0.0f, 0.5f);
  for (int i = 0; i < 100; i++) {
    curve.position(evalBezier(points, i / 99.0f));
  }
  curve.end();

  std::vector<Vec3f> points1;
  points1.push_back(Vec3f(-1.0f, 0.0f, 0.5f));
  points1.push_back(Vec3f(-1.0f, 1.0f, 0.5f));
  points1.push_back(Vec3f(0.0f, 1.0f, 0.5f));
  points1.push_back(Vec3f(0.0f, 0.0f, 0.5f));
  Geom hull1;
  hull1.begin(GL_LINE_STRIP);
  hull1.color(0.0f, 0.0f, 0.0f);
  for (auto v : points1) {
    hull1.position(v);
  }
  hull1.end();

  Geom curve1;
  curve1.begin(GL_LINE_STRIP);
  curve1.color(0.0f, 1.0f, 1.0f);
  for (int i = 0; i < 100; i++) {
    curve1.position(evalDeCast(points1, i / 99.0f));
  }
  curve1.end();

  glLineWidth(3);
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
      scene.camPos = scene.camPose.t;
    }

    scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t, Vec3f(0, 1, 0));
    scene.camPos = scene.camPose.t;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(0.25f, 0.25f, 0.25f, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    float aspect = float(width) / float(height);
    scene.projMat = Perspective(fovy, aspect, 0.1f, 100.0f);

    if (scene.camPose.t.y <= 0.0f) {
      grid.draw(scene, program);
    } else {
      square.draw(scene, litTexProgram);
    }
    cube.draw(scene, litTexProgram);
    sph.draw(scene, litTexProgram);
    tor.draw(scene, litTexProgram);

    light.obj.modelPose.t = scene.lightPos;
    light.draw(scene, program);

    hull.draw(scene, program);
    curve.draw(scene, program);

    hull1.draw(scene, program);
    curve1.draw(scene, program);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}
