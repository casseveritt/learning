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
#include "square.h"
#include "stb.h"
#include "tetra.h"
#include "torus.h"

#include "render.h"

using namespace r3;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

struct RendererImpl : public Renderer {

  RendererImpl() {}

  void Init() override;
  void Draw() override;
  void SetWindowSize(int w, int h) override;
  void SetCursorPos(Vec2d cursorPos) override;
  void ResetSim() override;

  Scene scene;
  Tetra dots;
  GLuint defaultVab;

  // programs init begin
  Prog program;
  Prog litProgram;
  Prog texProgram;
  Prog coordProgram;
  Prog litTexProgram;
  Prog spotProgram;

  // Texture begin
  GLuint check, brick, stone, wood;

  // objects init begin
  Geom grid;
  Square squ;
  Planef ground;

  Cube cube;

  Sphere sph;

  Torus tor;

  Sphere light;

  std::vector<Vec3f> points;

  Geom hull;

  Geom curve;

  Geom ray;
  Sphere intPoint;
  int hitID;
  Vec3f intObjLoc;

  int width;
  int height;

  Vec2d currPos;
};

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

void RendererImpl::Init() {
  scene.camPose.t.z = 2.0;

  scene.lightPose.r = Quaternionf(Vec3f(1, 0, 0), ToRadians(-90.0f));
  scene.lightPose.t = Vec3f(0, 1, 0);
  scene.lightCol = Vec3f(1, 1, 1);

  GLint version_maj = 0;
  GLint version_min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &version_maj);
  glGetIntegerv(GL_MINOR_VERSION, &version_min);
  printf("GL Version: %d.%d\n", version_maj, version_min);

  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray(defaultVab);

  // programs init begin
  program = Prog("Unlit");
  litProgram = Prog("Lit");
  texProgram = Prog("Tex");
  coordProgram = Prog("Coord");
  litTexProgram = Prog("LitTex");
  spotProgram = Prog("Spot");

  check = load_image("check.png");
  brick = load_image("bricks.png");
  stone = load_image("stone.png");
  wood = load_image("wood.png");
  // Texture end

  // objects init begin
  grid.begin(GL_LINES);
  grid.color(0.90f, 0.90f, 0.90f);
  static const int gridsize = 17;  // vertical or horizontal size *odd*
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
  float side = float((gridsize - 1) * s);

  squ.build(sqrDime, sqrSize, side);
  squ.obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
  squ.obj.shiny = 40.0f;
  squ.obj.tex = check;

  ground = Planef(Vec3f(0, 1, 0), 0.0f);

  cube.build(Matrix4f::Scale(0.375f));
  cube.obj.modelPose.t = Vec3f(-1, 0.375f, -1);
  cube.obj.matDifCol = Vec3f(0.7f, 0.0f, 0.0f);
  cube.obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
  cube.obj.shiny = 7.5f;
  cube.obj.tex = brick;

  sph.build(0.5f);
  sph.obj.modelPose.t = Vec3f(1.0, 0.5, -1.0);
  sph.obj.matDifCol = Vec3f(0.0f, 0.3f, 0.0f);
  sph.obj.matSpcCol = Vec3f(0.2f, 1.0f, 0.2f);
  sph.obj.shiny = 25.0f;
  sph.obj.tex = stone;

  tor.build(0.5f, 0.25f);
  tor.obj.modelPose.t = Vec3f(1, 0.25f, 1);
  tor.obj.matDifCol = Vec3f(0.0f, 0.0f, 0.5f);
  tor.obj.matSpcCol = Vec3f(0.3f, 0.3f, 1.0f);
  tor.obj.shiny = 15.0f;
  tor.obj.tex = wood;

  light.build(0.03125f);
  light.obj.modelPose.t = Vec3f(0.0f, 1.0f, 0.0f);

  points.push_back(Vec3f(-1.0f, 0.0f, 1.0f));
  points.push_back(Vec3f(-1.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 0.0f, 1.0f));

  hull.begin(GL_LINE_STRIP);
  hull.color(0.0f, 0.0f, 0.0f);
  for (auto v : points) {
    hull.position(v);
  }
  hull.end();

  curve.begin(GL_LINE_STRIP);
  curve.color(1.0f, 0.0f, 0.5f);
  for (int i = 0; i < 100; i++) {
    curve.position(evalDeCast(points, i / 99.0f));
  }
  curve.end();

  dots.build(50);
  dots.move(Vec3f(-0.75, 0.5, 0.75));

  glLineWidth(3);

  intPoint.build(0.015f, Vec3f(0.9, 0.0, 0.7));
  hitID = -1;
}

void RendererImpl::SetWindowSize(int w, int h){
  width = w;
  height = h;
}

void RendererImpl::SetCursorPos(Vec2d cursorPos){
  currPos = cursorPos;
}

void RendererImpl::ResetSim(){
  dots.reset();
}

void RendererImpl::Draw() {

  scene.camPose.t.x = sin(theta) * rad;
  scene.camPose.t.z = cos(theta) * rad;
  scene.camPose.t.y -= diffPos.y * 0.0125f;
  scene.camPos = scene.camPose.t;

  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t, Vec3f(0, 1, 0));
  scene.camPos = scene.camPose.t;  // Look into why I need this


  if (intersect) {
    currPos.y = (height - 1) - currPos.y;
    currPos.y = (currPos.y / (height - 1)) * 2 - 1;
    currPos.x = (currPos.x / (width - 1)) * 2 - 1;
    Vec4f nearInClip = Vec4f(currPos.x, currPos.y, -1.0, 1.0);
    Vec4f nearInCam = scene.projMat.Inverted() * nearInClip;
    nearInCam /= nearInCam.w;
    Vec4f farInClip = Vec4f(currPos.x, currPos.y, 1.0, 1.0);
    Vec4f farInCam = scene.projMat.Inverted() * farInClip;
    farInCam /= farInCam.w;
    Vec4f nearInWorld = scene.camPose.GetMatrix4() * nearInCam;
    Vec4f farInWorld = scene.camPose.GetMatrix4() * farInCam;
    Vec3f nearInWorld3 = Vec3f(&nearInWorld.x);
    Vec3f farInWorld3 = Vec3f(&farInWorld.x);
    Vec3f i;
    Planef plane(Vec3f(0, 1, 0), intPoint.obj.modelPose.t.y);
    Linef line(nearInWorld3, farInWorld3);
    bool hit = plane.Intersect(line, i);
    if (hitID == 0) {
      light.obj.modelPose.t.x += (i.x - light.obj.modelPose.t.x + intObjLoc.x);
      light.obj.modelPose.t.z += (i.z - light.obj.modelPose.t.z + intObjLoc.x);
    }
    if (hitID == 1) {
      sph.obj.modelPose.t.x += (i.x - sph.obj.modelPose.t.x + intObjLoc.x);
      sph.obj.modelPose.t.z += (i.z - sph.obj.modelPose.t.z + intObjLoc.x);
    }
    if (hitID == 2) {
      squ.obj.modelPose.t.x += (i.x - squ.obj.modelPose.t.x + intObjLoc.x);
      squ.obj.modelPose.t.z += (i.z - squ.obj.modelPose.t.z + intObjLoc.x);
      grid.modelPose.t = squ.obj.modelPose.t;
    }
    if (hitID == 3) {
      cube.obj.modelPose.t.x += (i.x - cube.obj.modelPose.t.x + intObjLoc.x);
      cube.obj.modelPose.t.z += (i.z - cube.obj.modelPose.t.z + intObjLoc.x);
    }
    if (hitID == 4) {
      tor.obj.modelPose.t.x += (i.x - tor.obj.modelPose.t.x + intObjLoc.x);
      tor.obj.modelPose.t.z += (i.z - tor.obj.modelPose.t.z + intObjLoc.x);
    }
  }

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  float aspect = float(width) / float(height);
  scene.projMat = Perspective(fovy, aspect, 0.1f, 100.0f);

  if (clickRay) {
    currPos.y = (height - 1) - currPos.y;
    currPos.y = (currPos.y / (height - 1)) * 2 - 1;
    currPos.x = (currPos.x / (width - 1)) * 2 - 1;
    Vec4f nearInClip = Vec4f(currPos.x, currPos.y, -1.0, 1.0);
    Vec4f nearInCam = scene.projMat.Inverted() * nearInClip;
    nearInCam /= nearInCam.w;
    Vec4f farInClip = Vec4f(currPos.x, currPos.y, 1.0, 1.0);
    Vec4f farInCam = scene.projMat.Inverted() * farInClip;
    farInCam /= farInCam.w;
    Vec4f nearInWorld = scene.camPose.GetMatrix4() * nearInCam;
    Vec4f farInWorld = scene.camPose.GetMatrix4() * farInCam;
    ray.begin(GL_LINES);
    ray.color(0.0f, 1.0f, 0.0f);
    ray.position(nearInWorld.x, nearInWorld.y, nearInWorld.z);
    ray.color(0.0f, 1.0f, 0.0f);
    ray.position(farInWorld.x, farInWorld.y, farInWorld.z);
    ray.end();
    Vec3f nearInWorld3 = Vec3f(&nearInWorld.x);
    Vec3f farInWorld3 = Vec3f(&farInWorld.x);
    Vec3f intLoc;
    Vec3f objIntLoc;

    float distance = (farInWorld3 - nearInWorld3).Length();

    if (light.intersect(nearInWorld3, farInWorld3, objIntLoc)) {
      if ((objIntLoc - nearInWorld3).Length() < distance) {
        distance = (objIntLoc - nearInWorld3).Length();
        intObjLoc = light.obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitID = 0;
      }
    }
    if (sph.intersect(nearInWorld3, farInWorld3, objIntLoc)) {
      if ((objIntLoc - nearInWorld3).Length() < distance) {
        distance = (objIntLoc - nearInWorld3).Length();
        intObjLoc = sph.obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitID = 1;
      }
    }
    if (squ.intersect(nearInWorld3, farInWorld3, objIntLoc)) {
      if ((objIntLoc - nearInWorld3).Length() < distance) {
        distance = (objIntLoc - nearInWorld3).Length();
        intObjLoc = squ.obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitID = 2;
      }
    }
    if (cube.intersect(nearInWorld3, farInWorld3, objIntLoc)) {
      if ((objIntLoc - nearInWorld3).Length() < distance) {
        distance = (objIntLoc - nearInWorld3).Length();
        intObjLoc = cube.obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitID = 3;
      }
    }
    if (tor.intersect(nearInWorld3, farInWorld3, objIntLoc)) {
      if ((objIntLoc - nearInWorld3).Length() < distance) {
        distance = (objIntLoc - nearInWorld3).Length();
        intObjLoc = tor.obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitID = 4;
      }
    }

    if (distance != (farInWorld3 - nearInWorld3).Length()) {
      intersect = true;
      intPoint.obj.modelPose.t = intLoc;
        // intObjLoc = intLoc;
      printf("intLoc: %.3f, %.3f, %.3f\n", intLoc.x, intLoc.y, intLoc.z);
    } else {
      intersect = false;
    }

    clickRay = false;
  }

  ray.draw(scene, program);

  if (scene.camPose.t.y <= 0.0f) {
    grid.draw(scene, program);
  } else {
    squ.draw(scene, litTexProgram);
  }
  if (intersect) {
    intPoint.draw(scene, program);
  }
  cube.draw(scene, litTexProgram);
  sph.draw(scene, litTexProgram);
  tor.draw(scene, litTexProgram);

  scene.lightPose.t = light.obj.modelPose.t;
  light.draw(scene, program);

  hull.draw(scene, program);
  curve.draw(scene, program);

  dots.draw(scene, litProgram, iterate);
}