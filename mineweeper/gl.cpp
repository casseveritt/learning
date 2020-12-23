#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

#include "geom.h"
#include "learning.h"
#include "linear.h"
#include "prog.h"
#include "render.h"
#include "scene.h"
#include "square.h"
#include "stb.h"

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

  Scene scene;
  GLuint defaultVab;

  Prog constColorProg;

  Geom grid;
  Planef ground;

  std::vector<Shape*> list;

  Geom ray;

  Shape* hitShape;
  Vec3f intObjLoc;

  int width;
  int height;

  Vec2d currPos;

  Vec3f nearInWorld3;
  Vec3f farInWorld3;
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
      int ij = i+j*w;
      /*
      if( (i % 32) == 0 ) {
        imgi[ij] = 0xff00ff00;
      } else if ( (j % 32) == 0) {
        imgi[ij] = 0xff0000ff;
      } else */
      {
        imgi[ij] = uint32_t(img[ij * n + 0]) << 0 | uint32_t(img[ij * n + 1]) << 8 | uint32_t(img[ij * n + 2]) << 16;
      }
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
  constColorProg = Prog("ccol");

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

  ground = Planef(Vec3f(0, 1, 0), 0.0f);

  glLineWidth(3);
}

void RendererImpl::SetWindowSize(int w, int h) {
  width = w;
  height = h;
}

void RendererImpl::SetCursorPos(Vec2d cursorPos) {
  currPos = cursorPos;
}

void RendererImpl::Draw() {
  scene.camPose.t.x = sin(theta) * rad;
  scene.camPose.t.z = cos(theta) * rad;
  scene.camPose.t.y = camHeight;
  scene.camPos = scene.camPose.t;

  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t, Vec3f(0, 1, 0));
  scene.camPos = scene.camPose.t;  // Look into why I need this

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  float aspect = float(width) / float(height);
  scene.projMat = Perspective(fovy, aspect, 0.1f, 100.0f);

  grid.draw(scene, constColorProg);
}