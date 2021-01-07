#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cstring>

#include "board.h"
#include "geom.h"
#include "learning.h"
#include "linear.h"
#include "multrect.h"
#include "prog.h"
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

  void Init(const Board& b) override;
  void Draw(const Board& b) override;
  void SetWindowSize(int w, int h) override;
  void SetCursorPos(Vec2d cursorPos) override;

  Scene scene;
  GLuint defaultVab;

  Prog constColorProg, texProg;

  GLuint zero, one, two, three, four, five, six, seven, eight;
  GLuint unrev, flag, mine, clickMine;

  MultRect tiles;

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

void RendererImpl::Init(const Board& b) {
  GLint version_maj = 0;
  GLint version_min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &version_maj);
  glGetIntegerv(GL_MINOR_VERSION, &version_min);
  printf("GL Version: %d.%d\n", version_maj, version_min);

  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray(defaultVab);

  // Programs init begin
  constColorProg = Prog("ccol");
  texProg = Prog("tex");

  // Textures init begin
  zero = load_image("0tile.png");
  one = load_image("1tile.png");
  two = load_image("2tile.png");
  three = load_image("3tile.png");
  four = load_image("4tile.png");
  five = load_image("5tile.png");
  six = load_image("6tile.png");
  seven = load_image("7tile.png");
  eight = load_image("8tile.png");

  unrev = load_image("unrevealed.png");
  flag = load_image("flagged.png");
  mine = load_image("mine.png");
  clickMine = load_image("clickedMine.png");

  tiles.s0 = 1.0f / (b.width);
  tiles.s1 = 1.0f / (b.height);
}

void RendererImpl::SetWindowSize(int w, int h) {
  width = w;
  height = h;
}

void RendererImpl::SetCursorPos(Vec2d cursorPos) {
  currPos = cursorPos;
}

static double GetTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

void RendererImpl::Draw(const Board& b) {
  static int frames = 0;
  static double sumtime = 0.0;

  double t0 = GetTimeInSeconds();

  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -scene.camPose.t, Vec3f(0, 1, 0));

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  // float aspectRatio = float(width) / height;
  scene.projMat = Ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

  GLuint tex[] = {zero, one, two, three, four, five, six, seven, eight, unrev, flag, mine, clickMine};

  for (int i = 0; i < 13; i++) {
    std::vector<MultRect::Rect> rects;
    tiles.obj.tex = tex[i];
    for (int x = 0; x < b.width; x++) {
      for (int y = 0; y < b.height; y++) {
        Board::Tile t = b.el(x, y);
        MultRect::Rect r(x, y);
        if (i == t.adjMines && t.revealed) {
          rects.push_back(r);
        } else if (i == 9 && !t.revealed) {
          rects.push_back(r);
        } else if (i == 10 && t.flagged) {
          rects.push_back(r);
        } else if (i == 11 && t.isMine && t.revealed) {
          rects.push_back(r);
        } else if (i == 12 && t.isMine && t.revealed && t.flagged) {
          rects.push_back(r);
        }
        // rects.push_back(MultRect::Rect(x,y));
        // setTile(b.el(x, y), xSide, ySide);
        // rect.obj.modelPose.t = Vec3f((xSide * x), (ySide * (b.height - (1 + y))), 0.0f);
        // rect.draw(scene, texProg);
      }
    }
    tiles.build(rects);
    tiles.draw(scene, texProg);
  }

  double t1 = GetTimeInSeconds();
  frames++;
  sumtime += (t1 - t0);
  if (frames >= 100) {
    //printf("avg frame time = %d msec\n", int((sumtime / frames) * 1000));
    //printf("avg fps = %d\n", int((frames / (sumtime))));
    frames = 0;
    sumtime = 0.0;
  }
}
