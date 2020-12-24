#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstring>

#include "board.h"
#include "geom.h"
#include "learning.h"
#include "linear.h"
#include "prog.h"
#include "rectangle.h"
#include "render.h"
#include "scene.h"
#include "stb.h"

using namespace r3;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

struct RendererImpl : public Renderer {
  RendererImpl() {}

  void Init() override;
  void Draw(const Board& b) override;
  void SetWindowSize(int w, int h) override;
  // void Click(int w, int h) override;
  void SetCursorPos(Vec2d cursorPos) override;

  Scene scene;
  GLuint defaultVab;

  Prog constColorProg;

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
      int ij = i + j * w;
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

void RendererImpl::Init() {
  GLint version_maj = 0;
  GLint version_min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &version_maj);
  glGetIntegerv(GL_MINOR_VERSION, &version_min);
  printf("GL Version: %d.%d\n", version_maj, version_min);

  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray(defaultVab);

  // programs init begin
  constColorProg = Prog("ccol");
}

void RendererImpl::SetWindowSize(int w, int h) {
  width = w;
  height = h;
}

void RendererImpl::SetCursorPos(Vec2d cursorPos) {
  currPos = cursorPos;
}

/*
void RendererImpl::Click(int w, int h) {
  currPos.y = (h - 1) - currPos.y;
  currPos.y = (currPos.y / (h - 1)) * 2 - 1;
  currPos.x = (currPos.x / (w - 1)) * 2 - 1;
  Vec4f nearInClip = Vec4f(currPos.x, currPos.y, -1.0, 1.0);
  Vec4f nearInCam = scene.projMat.Inverted() * nearInClip;
  nearInCam /= nearInCam.w;
  Vec4f farInClip = Vec4f(currPos.x, currPos.y, 1.0, 1.0);
  Vec4f farInCam = scene.projMat.Inverted() * farInClip;
  farInCam /= farInCam.w;
  nIW3 = Homogenize(scene.camPose.GetMatrix4() * nearInCam);
  fIW3 = Homogenize(scene.camPose.GetMatrix4() * farInCam);

  Vec3f intLoc;
  Vec3f objIntLoc;

  float distance = (fIW3 - nIW3).Length();

  for (auto shape : board) { // Need to add interesect for board to use here
    if (shape->intersect(nIW3, fIW3, objIntLoc)) {
      if ((objIntLoc - nIW3).Length() < distance) {
        distance = (objIntLoc - nIW3).Length();
        intObjLoc = shape->obj.modelPose.t - objIntLoc;
        intLoc = objIntLoc;
        hitShape = shape;
      }
    }
  }
}
*/

static Rectangle makeTile(const Board::Tile& t, Vec3f pos, float xSide, float ySide) {
  Vec3f tileCol(0.9f, 0.9f, 0.9f);
  if (t.revealed) {
    if (t.isMine) {
      tileCol = Vec3f(0.8f, 0.2f, 0.2f);
    } else {
      switch (t.adjMines) {
        case 0:
          tileCol = Vec3f(0.7f, 0.7f, 0.7f);
          break;
        case 1:
          tileCol = Vec3f(0.4f, 0.4f, 0.8f);
          break;
        case 2:
          tileCol = Vec3f(0.4f, 0.8f, 0.4f);
          break;
        case 3:
          tileCol = Vec3f(0.8f, 0.5f, 0.1f);
          break;
        case 4:
          tileCol = Vec3f(0.1f, 0.1f, 0.4f);
          break;
        case 5:
          tileCol = Vec3f(0.55f, 0.1f, 0.1f);
          break;
        case 6:
          tileCol = Vec3f(0.2f, 0.8f, 0.8f);
          break;
        case 7:
          tileCol = Vec3f(0.05f, 0.05f, 0.05f);
          break;
        case 8:
          tileCol = Vec3f(0.4f, 0.4f, 0.4f);
          break;
        default:
          break;
      }
    }
  } else if (t.flagged) {
    tileCol = Vec3f(0.8f, 0.8f, 0.2f);
  }
  Rectangle rect;
  rect.build(xSide, ySide, tileCol);
  rect.obj.modelPose.t = pos;
  return rect;
}

void RendererImpl::Draw(const Board& b) {
  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t, Vec3f(0, 1, 0));

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  float aspectRatio = float(width) / height;
  scene.projMat = Ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

  float xSide = 1.0f / (b.width);
  float ySide = 1.0f / (b.height);
  for (int x = 0; x < b.width; x++) {
    for (int y = 0; y < b.height; y++) {
      Rectangle rect = makeTile(b.el(x, y), Vec3f((xSide * x), (ySide * (b.height - (1 + y))), 0.0f), xSide, ySide);
      rect.draw(scene, constColorProg);
    }
  }
}