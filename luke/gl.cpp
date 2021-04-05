#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>
#include <string>

#include "cube.h"
#include "geom.h"
#include "learning.h"
#include "linear.h"
#include "plyobj.h"
#include "prog.h"
#include "render.h"
#include "scene.h"
#include "sphere.h"
#include "square.h"
#include "stb.h"
#include "tetra.h"
#include "torus.h"

using namespace r3;
using namespace std;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

struct RendererImpl : public Renderer {
  RendererImpl() {}

  void Init() override;
  void Draw() override;
  void SetCursorPos(Vec2d cursorPos) override;
  void ResetSim() override;
  void RayInWorld(Vec3f& nIW3, Vec3f& fIW3) override;
  void Intersect() override;
  void Drag(Vec3f newPos) override;

  Scene scene;
  Tetra dots;
  GLuint defaultVab;

  Prog program, litProgram, texProgram, coordProgram, litTexProgram, spotProgram;

  GLuint check, brick, stone, wood;
  Planef ground;

  std::vector<Shape*> list;

  std::vector<Vec3f> points;
  Geom grid, curve, ray;
  Sphere intPoint;

  Shape* hitShape;

  Vec2d currPos;

  Vec3f nearInWorld3, farInWorld3;
};

Renderer* CreateRenderer() {
  return new RendererImpl();
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

  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      int ij = i + j * w;
      /*
      if( (i % 32) == 0 ) {
        imgi[ij] = 0xff00ff00;
      } else if ( (j % 32) == 0) {
        imgi[ij] = 0xff0000ff;
      } else */
      { imgi[ij] = uint32_t(img[ij * n + 0]) << 0 | uint32_t(img[ij * n + 1]) << 8 | uint32_t(img[ij * n + 2]) << 16; }
    }
  }

  glGenTextures(1, &out);
  glBindTexture(GL_TEXTURE_2D, out);
  glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, w, h);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, imgi);
  glGenerateMipmap(GL_TEXTURE_2D);
  delete[] imgi;
  image_free(img);
  return out;
}

/*static Vec3f evalBezier(const std::vector<Vec3f>& p, float t) {
  float t2 = t * t;
  float t3 = t * t * t;
  float w[4] = {-1 * t3 + 3 * t2 - 3 * t + 1, 3 * t3 - 6 * t2 + 3 * t, -3 * t3 + 3 * t2, t3};
  return p[0] * w[0] + p[1] * w[1] + p[2] * w[2] + p[3] * w[3];
}*/

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

  {
    auto squ = new Square;  // Ground Square
    squ->build(sqrDime, sqrSize, side);
    list.push_back(squ);
    squ->obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
    squ->obj.shiny = 40.0f;
    squ->obj.tex = check;
  }

  ground = Planef(Vec3f(0, 1, 0), 0.0f);

  {
    auto cube = new Cube;  // Cube
    cube->build(Matrix4f::Scale(0.375f));
    list.push_back(cube);
    list.back()->obj.modelPose.t = Vec3f(-1, 0.375f, -1);
    list.back()->obj.matDifCol = Vec3f(0.7f, 0.0f, 0.0f);
    list.back()->obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.25f);
    list.back()->obj.shiny = 7.5f;
    list.back()->obj.tex = brick;
  }

  {
    auto sph = new Sphere;  // Sphere
    sph->build(0.5f);
    list.push_back(sph);
    list.back()->obj.modelPose.t = Vec3f(1.0, 0.5, -1.0);
    list.back()->obj.matDifCol = Vec3f(0.0f, 0.3f, 0.0f);
    list.back()->obj.matSpcCol = Vec3f(0.2f, 1.0f, 0.2f);
    list.back()->obj.shiny = 25.0f;
    list.back()->obj.tex = stone;
  }

  {
    auto tor = new Torus;  // Torus
    tor->build(0.5f, 0.25f);
    list.push_back(tor);
    list.back()->obj.modelPose.t = Vec3f(1, 0.25f, 1);
    list.back()->obj.matDifCol = Vec3f(0.0f, 0.0f, 0.5f);
    list.back()->obj.matSpcCol = Vec3f(0.3f, 0.3f, 1.0f);
    list.back()->obj.shiny = 15.0f;
    list.back()->obj.tex = wood;
  }

  {
    auto cow = new Plyobj;  // Cow
    cow->build(fopen("models/cow.ply", "r"), Matrix4f::Scale(0.5f));
    list.push_back(cow);
    // list.back()->obj.modelPose.t = Vec3f(-0.8f, 0.0f, 1.0f);
    list.back()->obj.matDifCol = Vec3f(0.2f, 0.1f, 0.8f);
    list.back()->obj.matSpcCol = Vec3f(0.25f, 0.25f, 0.95f);
    list.back()->obj.shiny = 1.5f;
  }

  {
    auto light = new Sphere;  // Light Sphere
    light->build(0.03125f);
    list.push_back(light);
    list.back()->obj.modelPose.t = Vec3f(0.0f, 2.0f, 0.0f);
  }

  points.push_back(Vec3f(-1.0f, 0.0f, 1.0f));
  points.push_back(Vec3f(-1.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 1.0f, 1.0f));
  points.push_back(Vec3f(0.0f, 0.0f, 1.0f));

  curve.begin(GL_LINE_STRIP);
  curve.color(0.0f, 0.0f, 0.0f);
  for (auto v : points) {
    curve.position(v);
  }
  curve.color(1.0f, 0.0f, 0.5f);
  for (int i = 0; i < 100; i++) {
    curve.position(evalDeCast(points, i / 99.0f));
  }
  curve.end();

  dots.build(50);
  dots.move(Vec3f(-0.75, 0.5, 0.75));

  glLineWidth(3);

  intPoint.build(0.015f, Vec3f(0.9, 0.0, 0.7));
}

void RendererImpl::SetCursorPos(Vec2d cursorPos) {
  currPos = cursorPos;
}

void RendererImpl::ResetSim() {
  dots.reset();
}

void RendererImpl::RayInWorld(Vec3f& nIW3, Vec3f& fIW3) {
  currPos.y = (height - 1) - currPos.y;
  currPos.y = (currPos.y / (height - 1)) * 2 - 1;
  currPos.x = (currPos.x / (width - 1)) * 2 - 1;
  Vec4f nearInClip = Vec4f(currPos.x, currPos.y, -1.0, 1.0);
  Vec4f nearInCam = scene.projMat.Inverted() * nearInClip;
  nearInCam /= nearInCam.w;
  Vec4f farInClip = Vec4f(currPos.x, currPos.y, 1.0, 1.0);
  Vec4f farInCam = scene.projMat.Inverted() * farInClip;
  farInCam /= farInCam.w;
  nIW3 = Homogenize(scene.camPose.GetMatrix4() * nearInCam);
  fIW3 = Homogenize(scene.camPose.GetMatrix4() * farInCam);
  ray.begin(GL_LINES);
  ray.color(0.0f, 1.0f, 0.0f);
  ray.position(nIW3.x, nIW3.y, nIW3.z);
  ray.color(0.0f, 1.0f, 0.0f);
  ray.position(fIW3.x, fIW3.y, fIW3.z);
  ray.end();
}

void RendererImpl::Intersect() {
  Vec3f nIW3, fIW3;
  RayInWorld(nIW3, fIW3);
  Vec3f objIntLoc;

  float distance = (fIW3 - nIW3).Length();

  for (auto shape : list) {
    if (shape->intersect(nIW3, fIW3, objIntLoc)) {
      if ((objIntLoc - nIW3).Length() < distance) {
        distance = (objIntLoc - nIW3).Length();
        // intObjLoc = shape->obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitShape = shape;
      }
    }
  }

  if (distance != (fIW3 - nIW3).Length()) {
    intersect = true;
    intPoint.obj.modelPose.t = intLoc;
  } else {
    hitShape = nullptr;
    intersect = false;
  }
}

void RendererImpl::Drag(Vec3f newPos) {
  intPoint.obj.modelPose.t = newPos;
  float x = newPos.x - intLoc.x;
  float z = newPos.z - intLoc.z;
  hitShape->obj.modelPose.t.x += x;
  hitShape->obj.modelPose.t.z += z;
  intLoc = newPos;
}

void RendererImpl::Draw() {
  scene.camPose = camera;
  scene.camPos = scene.camPose.t;

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 0);
  if (!trackCamera) {
    glClearColor(0.5f, 0.5f, 0.5f, 0);
  }
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  float aspect = float(width) / float(height);
  scene.projMat = Perspective(fovy, aspect, 0.1f, 100.0f);

  ray.draw(scene, program);

  if (scene.camPose.t.y <= 0.0f) {
    grid.draw(scene, program);
  } else {
    list[0]->draw(scene, litTexProgram);
  }
  if (intersect) {
    intPoint.draw(scene, program);
  }
  /*for (int i = 1; i < 4; i++) {
    list[i]->draw(scene, litTexProgram);
  }*/

  list[4]->draw(scene, litProgram);

  scene.lightPose.t = list[5]->obj.modelPose.t;
  list[5]->draw(scene, program);

  // curve.draw(scene, program);

  // dots.draw(scene, litProgram, iterate);
}