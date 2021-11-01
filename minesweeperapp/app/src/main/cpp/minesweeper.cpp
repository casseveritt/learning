/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// Build:   ./gradlew build
// Install: adb install ./app/build/outputs/apk/debug/app-debug.apk
// Save logs to foo.log: adb lolcat -v time | tee foo.log

#include <jni.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>

#include "board.h"
#include "geom.h"
#include "minesweeper.h"
#include "linear.h"
#include "multrect.h"
#include "render.h"
#include "scene.h"
#include "stb.h"

using namespace r3;

std::string baseDir;
jobject appActivity;
JavaVM * jvm = NULL;
JNIEnv * appEnv = NULL;

bool checkGlError(const char* funcName) {  // Checks if there have been errors
  GLint err = glGetError();
  if (err != GL_NO_ERROR) {
    ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
    return true;
  }
  return false;
}

static void printString(const char* name, GLenum s) {  // Prints out to log
  const char* v = (const char*)glGetString(s);
  ALOGV("App %s: %s\n", name, v);  // Just like printf
}

enum Space {
  Space_Pixel,
  Space_Screen,
  Space_Window,
  Space_Board,
  Space_Tile,
  Space_Reset,
  Space_Toggle
};

enum TouchState {
  Idle,
  Touching,
  Holding,
  Dragging,
  Pinching,
  WaitTillEnd,
  TouchingBanner
};

struct UScaleBias2f {
  float scale = 1.0f;
  Vec2f trans;
  Matrix4f GetMatrix4() const {
    Matrix4f s = Matrix4f::Scale(scale);
    Matrix4f t = Matrix4f::Translate(Vec3f(trans.x, trans.y, 0.0f));
    return t * s;
  }
  Matrix4f GetMatrix4Inverted() const {
    Matrix4f s = Matrix4f::Scale(1.0f/scale);
    Matrix4f t = Matrix4f::Translate(Vec3f(-trans.x, -trans.y, 0.0f));
    return s * t;
  }
};

struct Transforms {

  Transforms() {
    xfResetFromScreen = Matrix4f::Identity();
    xfToggleFromScreen = Matrix4f::Identity();
    xfScreenFromPixel = Matrix4f::Identity();
    xfBoardTileFromBoard = Matrix4f::Identity();
    xfWindowFromScreen = Matrix4f::Identity();
  }
  void SetPixels(int w, int h) {
    auto s = Matrix4f::Scale(Vec3f((1.0f/w), -(1.0f/w),1));
    auto t = Matrix4f::Translate(Vec3f(0,(float(h)/w),0));
    xfScreenFromPixel = t * s;
  }
  void SetTiles(int r, int c) {
    xfBoardTileFromBoard.SetScale(fmax(r, c));
  }
  Matrix4f transform(Space to, Space from) {
    if (from == Space_Tile && to == Space_Pixel) {
      return (xfBoardTileFromBoard * xfBoardFromScreen.GetMatrix4() * xfScreenFromPixel).Inverted();
    }
    if (from == Space_Tile && to == Space_Screen) {
      return (xfBoardTileFromBoard * xfBoardFromScreen.GetMatrix4()).Inverted();
    }
    if (from == Space_Pixel && to == Space_Board) {
      return (xfBoardFromScreen.GetMatrix4() * xfScreenFromPixel);
    }
    if (from == Space_Board && to == Space_Pixel) {
      return (xfBoardFromScreen.GetMatrix4() * xfScreenFromPixel).Inverted();
    }
    if (from == Space_Pixel && to == Space_Screen) {
      return xfScreenFromPixel;
    }
    if (from == Space_Pixel && to == Space_Tile) {
      return (xfBoardTileFromBoard * xfBoardFromScreen.GetMatrix4() * xfScreenFromPixel);
    }
    if (from == Space_Board && to == Space_Screen) {
      return xfBoardFromScreen.GetMatrix4Inverted();
    }
    if (from == Space_Pixel && to == Space_Reset) {
      return xfResetFromScreen * xfScreenFromPixel;
    }
    if (from == Space_Pixel && to == Space_Toggle) {
      return xfToggleFromScreen * xfScreenFromPixel;
    }
    ALOGE("Transforms::transform unsupported transform to %d from %d", to, from);
    exit(42);
    return Matrix4f();
  }

  UScaleBias2f xfBoardFromScreen;

  Matrix4f xfResetFromScreen;
  Matrix4f xfToggleFromScreen;
  Matrix4f xfScreenFromPixel;
  Matrix4f xfWindowFromScreen;
  Matrix4f xfBoardTileFromBoard;
};

static double GetTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

struct RendererImpl : public Renderer {
  RendererImpl() {}

  void Init() override;
  void Draw() override;
  void SetWindowSize(int w, int h) override;
  void SetCursorPos(Vec2d cursorPos) override;
  void Touch(float x, float y, int type, int index) override;

  Transforms xf;

  TouchState ts = Idle;
  float startDist;
  Matrix4f prevScreenFromBoard;
  Vec3f touch[2], touchStart[2], dragStart, lastTouch[2];
  Vec3f centerInBoard;
  float scaleFactor;
  int framesheld = 0;
  float screenHeight = 0.0f;

  Board board;
  int bWidth = 10, bHeight = 10, mines = 10;
  bool tapFlag = false;

  Scene scene;
  GLuint defaultVab;
  double startTime, timeElapsed;

  Prog constColorProg, texProg;

  GLuint zero, one, two, three, four, five, six, seven, eight;
  GLuint unrev, flag, mine, clickMine;
  GLuint timer0, timer1, timer2, timer3, timer4, timer5, timer6, timer7, timer8, timer9; 

  MultRect tiles;
  Rectangle banner, timer, resetButton, toggleButton, digit1, digit2, digit3;

  int width, height;

  Vec2d currPos;

  Vec3f nearInWorld3;
  Vec3f farInWorld3;
};

Renderer* CreateRenderer() {
  return new RendererImpl();
}

static GLuint load_image(const char* imgName) {
  int w, h, n;

  const char* im = baseDir.c_str();
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

// ----------------------------------------------------------------------------

void RendererImpl::Init() {

  srand(0); // Random board seed
  board.build(bWidth, bHeight, mines);

  ALOGV("Gl init");
  GLint version_maj = 0;
  GLint version_min = 0;
  glGetIntegerv(GL_MAJOR_VERSION, &version_maj);
  glGetIntegerv(GL_MINOR_VERSION, &version_min);
  printf("GL Version: %d.%d\n", version_maj, version_min);

  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray(defaultVab);

  // Programs init begin
  ALOGV("Programs init");
  constColorProg = Prog("ccol", baseDir);
  texProg = Prog("tex", baseDir);

  // Textures init begin
  ALOGV("Texture loading");
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

  timer0 = load_image("Timer0.png");
  timer1 = load_image("Timer1.png");
  timer2 = load_image("Timer2.png");
  timer3 = load_image("Timer3.png");
  timer4 = load_image("Timer4.png");
  timer5 = load_image("Timer5.png");
  timer6 = load_image("Timer6.png");
  timer7 = load_image("Timer7.png");
  timer8 = load_image("Timer8.png");
  timer9 = load_image("Timer9.png");

  ALOGV("Object building");
  xf.SetTiles(board.width, board.height);

  tiles.s0 = tiles.s1 = 1.0f;

  startTime = GetTimeInSeconds();
  timeElapsed = 0.0;

  ALOGV("Done initializing");
}

void RendererImpl::SetWindowSize(int w, int h) {
  width = w;
  height = h;
  xf.SetPixels(w, h);
  ALOGV("Screen Dimensions: Width = %d, Height = %d", w, h);
  float aspectRatio = float(h)/w;
  screenHeight = aspectRatio;

  banner.build(1.0f, 0.15f, Vec3f(0.8f, 0.8f, 0.8f));
  resetButton.build(0.125f, 0.125f, Vec3f(0.2f, 0.2f, 0.2f));
  toggleButton.build(0.075f, 0.075f, Vec3f(0.5f, 0.25f, 0.25f));

  digit1.build(0.05, 0.1, Vec3f(0.0f, 0.0f, 1.0f));
  digit2.build(0.05, 0.1, Vec3f(0.0f, 1.0f, 0.0f));
  digit3.build(0.05, 0.1, Vec3f(1.0f, 0.0f, 0.0f));

  banner.obj.model = Matrix4f::Translate(Vec3f(0.0f, aspectRatio-0.15f, 0.0f)) * banner.obj.model;
  resetButton.obj.model = Matrix4f::Translate(Vec3f(0.4375f, aspectRatio-0.1375f, 0.0f)) * resetButton.obj.model;
  toggleButton.obj.model = Matrix4f::Translate(Vec3f(0.725f, aspectRatio-0.1125f, 0.0f)) * toggleButton.obj.model;

  digit1.obj.model = Matrix4f::Translate(Vec3f(0.25, aspectRatio-0.125f, 0.0f)) * digit1.obj.model;
  digit2.obj.model = Matrix4f::Translate(Vec3f(0.20, aspectRatio-0.125f, 0.0f)) * digit2.obj.model;
  digit3.obj.model = Matrix4f::Translate(Vec3f(0.15, aspectRatio-0.125f, 0.0f)) * digit3.obj.model;

  Matrix4f s = Matrix4f::Scale(1.0f/0.125f);
  Matrix4f t = Matrix4f::Translate(Vec3f(-0.4375f, -(aspectRatio-0.1375f), 0.0f));
  xf.xfResetFromScreen = s * t;

  s = Matrix4f::Scale(1.0f/0.075f);
  t = Matrix4f::Translate(Vec3f(-0.725f, -(aspectRatio-0.1125f), 0.0f));
  xf.xfToggleFromScreen = s * t;
}

void RendererImpl::SetCursorPos(Vec2d cursorPos) {
  currPos = cursorPos;
}

void RendererImpl::Draw() {
  static int frames = 0; 
  static double sumtime = 0.0;

  double t0 = GetTimeInSeconds(); 

  scene.camPose.t.SetValue(0,0,1);
  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0));

  tiles.obj.model = xf.transform(Space_Screen, Space_Tile);

  //glViewport(0, 0, width, height);

  //glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  float aspectRatio = float(height) / width;
  scene.projMat = Ortho(0.0f, 1.0f, 0.0f, float(height)/width, -1.0f, 1.0f);

  GLuint tex[] = {zero, one, two, three, four, five, six, seven, eight, unrev, flag, mine, clickMine};

  for (int i = 0; i < 13; i++) {
    std::vector<MultRect::Rect> rects;
    tiles.obj.tex = tex[i];
    for (int x = 0; x < board.width; x++) {
      for (int y = 0; y < board.height; y++) {
        Board::Tile t = board.el(x, y);
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
    glScissor(0, 0, 1080, 1900);
    glEnable(GL_SCISSOR_TEST);
    tiles.draw(scene, texProg);
    glDisable(GL_SCISSOR_TEST);
  }

  double t1 = GetTimeInSeconds();
  frames++;
  sumtime += (t1 - t0);
  if (frames >= 100) {
    // printf("avg frame time = %d msec\n", int((sumtime / frames) * 1000));
    // printf("avg fps = %d\n", int((frames / (sumtime))));
    frames = 0;
    sumtime = 0.0;
  }

  if (board.state == 0) {
    startTime = GetTimeInSeconds();
  }
  if (board.state == 1) {
    timeElapsed = GetTimeInSeconds() - startTime;
  }
  int firstDig = fmod(timeElapsed, 10);
  int secondDig = fmod((timeElapsed / 10), 10);
  int thirdDig = fmod((timeElapsed / 100), 10);
  GLuint digitTextures[] = {timer0, timer1, timer2, timer3, timer4, timer5, timer6, timer7, timer8, timer9};
  digit1.obj.tex = digitTextures[firstDig];
  digit2.obj.tex = digitTextures[secondDig];
  digit3.obj.tex = digitTextures[thirdDig];
  
  banner.draw(scene, constColorProg);
  resetButton.draw(scene, constColorProg);
  toggleButton.draw(scene, constColorProg);

  digit1.draw(scene, texProg);
  digit2.draw(scene, texProg);
  digit3.draw(scene, texProg);

}

#define PTR_DOWN 0
#define PTR_UP 1
#define PTR_MOVE 2
#define PTR2_DOWN 261
#define PTR2_UP 262

void RendererImpl::Touch(float x, float y, int type, int index) {
  Vec3f posInPixels(x, y, 1.0f);
  ALOGV("touch: type = %d, index = %d, x = %.0f, y = %.0f", type, index, x, y);
  if (type == PTR_DOWN || type == PTR2_DOWN) {
    touchStart[index] = Vec3f(x, y, 0.0f);
    prevScreenFromBoard = xf.transform(Space_Screen, Space_Board);
  } else if (type == PTR_MOVE) {
    touch[index] = Vec3f(x, y, 0.0f);
  }

  if (index == 0) {
    ALOGV("TouchState: %d Type: %d", ts, type);
  }

  if (ts == Idle && type == PTR_DOWN) {
    framesheld = 0;
    ts = Touching;
  }

  if (ts == Touching) { // Tap
    framesheld++;
    float dragDist = (touchStart[0] - posInPixels).Length();
    if (framesheld > 14) {
      ts = Holding;
    } else if (dragDist >= 50) {
      dragStart = Vec3f(x, y, 0.0f);
      ts = Dragging;
    } else if (type == PTR2_DOWN) {
      ts = Pinching;
    }
    else if (type == PTR_UP) {
      Vec3f resetButtPos = xf.transform(Space_Reset, Space_Pixel) * posInPixels;
      Vec3f toggleButtPos = xf.transform(Space_Toggle, Space_Pixel) * posInPixels;
      if (1.0f > resetButtPos.x && resetButtPos.x > 0.0f && 1.0f > resetButtPos.y && resetButtPos.y > 0.0f) {
        board.reset();
        tapFlag = false;
        startTime = GetTimeInSeconds();                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
      } else if (1.0f > toggleButtPos.x && toggleButtPos.x > 0.0f && 1.0f > toggleButtPos.y && toggleButtPos.y > 0.0f) {
        tapFlag = !tapFlag;
      } else {
        Vec3f posInTiles = xf.transform(Space_Tile, Space_Pixel) * posInPixels;
        ALOGV("Flagging tile x: %f, y: %f", posInTiles.x, posInTiles.y);
        if (tapFlag) {
          board.flag(posInTiles.x, posInTiles.y);
        } else {
          board.reveal(posInTiles.x, posInTiles.y);
        }
        ts = Idle;
      }
    }
    lastTouch[0] = touch[0];
  }

  if (ts == Holding) { // Tap and hold in one place
    if (type == PTR2_DOWN) {
      ts = WaitTillEnd;
    } else {
      Vec3f posInTiles = xf.transform(Space_Tile, Space_Pixel) * posInPixels;
      // Should also highlight tile being held
      if (type == PTR_UP) {
        ALOGV("Revealing tile x: %f, y: %f", posInTiles.x, posInTiles.y);
        board.reveal(posInTiles.x, posInTiles.y);
        ts = Idle;
      }
    }
  }

  if (ts == Dragging) { // Pan around board
    if (type == PTR2_DOWN) {
      ts = WaitTillEnd;
    } else {
      Vec3f posInScreen = xf.transform(Space_Screen, Space_Pixel) * posInPixels;
      Vec3f dsInScreen = xf.transform(Space_Screen, Space_Pixel) * dragStart;
      Vec3f tvec = posInScreen - dsInScreen;
      Matrix4f tmat = Matrix4f::Translate(-tvec);
      Matrix4f xfBoardFromScreen = prevScreenFromBoard.Inverted() * tmat;
      Matrix4f xfScreenFromBoard = xfBoardFromScreen.Inverted();

      Vec3f blCornerInScreen = xfScreenFromBoard * Vec3f(0,0,0);
      Vec3f trCornerInScreen = xfScreenFromBoard * Vec3f(1,1,0);

      bool lInBounds = blCornerInScreen.x < 0.05f;
      bool bInBounds = blCornerInScreen.y < 0.05f;
      bool rInBounds = trCornerInScreen.x > 0.95f;
      bool tInBounds = trCornerInScreen.y > 0.95f;

      if (lInBounds && bInBounds && rInBounds && tInBounds) {
        xf.xfBoardFromScreen.scale = xfBoardFromScreen.m[0];
        xf.xfBoardFromScreen.trans.x = xfBoardFromScreen.m[12];
        xf.xfBoardFromScreen.trans.y = xfBoardFromScreen.m[13];
      }
      if (type == PTR_UP) {
        ts = Idle;
      }
    }
  }

  if (ts == Pinching) { // Scale gesture
    if (type == PTR2_DOWN && index == 1) {
      startDist = (touchStart[0] - touchStart[1]).Length();
      Vec3f centerInPix = (touchStart[0] + touchStart[1]) * 0.5f;
      centerInBoard = xf.transform(Space_Board, Space_Pixel) * centerInPix;
      lastTouch[1] = touch[1];
    }
    if (type == PTR_MOVE) {
      Matrix4f invCenterTrans = Matrix4f::Translate(-centerInBoard);
      Vec3f newCenterInBoard = (prevScreenFromBoard.Inverted() * xf.transform(Space_Screen, Space_Pixel)) * ((touch[0] + touch[1]) * 0.5f);
      Matrix4f centerTrans = Matrix4f::Translate(newCenterInBoard);
      float dist = (touch[0] - touch[1]).Length();
      scaleFactor = dist / startDist;

      // prevScreenFromBoard * ctrans * scale * inv(ctrans)
      ALOGV("Board Scale change by: %f", scaleFactor);
      Matrix4f s = Matrix4f::Scale(scaleFactor);
      Matrix4f scaleAboutCenter = centerTrans * s * invCenterTrans;
      Matrix4f xfScreenFromBoard = prevScreenFromBoard * scaleAboutCenter;
      Matrix4f xfBoardFromScreen = xfScreenFromBoard.Inverted();
      UScaleBias2f xfBfS = xf.xfBoardFromScreen;
      xfBfS.scale = xfBoardFromScreen.m[0];
      xfBfS.trans.x = xfBoardFromScreen.m[12];
      xfBfS.trans.y = xfBoardFromScreen.m[13];

      if (0.5f < xfBfS.scale && xfBfS.scale < 1.0f) {
        xf.xfBoardFromScreen = xfBfS;
      }
    }
    if (type == PTR2_UP || type == 6) {
      ts = WaitTillEnd;
    }
  }

  if (ts == WaitTillEnd && type == PTR_UP) {
    ts = Idle;
  }
}

// ----------------------------------------------------------------------------

/*
static JNIEnv *attachThread() {
  if( ! appActivity ) {
    return NULL;
  }
  JNIEnv *env = NULL;
  if( jvm == NULL || jvm->AttachCurrentThread( &env, NULL) || env == NULL ) {
    ALOGE( "Unable to attach the current thread!" );
  }
  return env;
}
*/

jstring appNewJavaString( JNIEnv *env, const std::string & s ) {
  return env->NewStringUTF( s.c_str() );
}

static jmethodID getMethod( JNIEnv *env, jobject obj, const char *methodName, const char *methodSignature ) {

  ALOGV( "Get Method %s\n", methodName );
  
  jclass objClass = env->GetObjectClass( obj );

  ALOGV( " 1 Get Method %s\n", methodName );
  
  jmethodID m = env->GetMethodID( objClass, methodName, methodSignature );
  ALOGV( " 2 Get Method %p\n", m );
  if ( m == NULL ) {
    ALOGE( "failed to find %s method (sig %s)\n", methodName, methodSignature );
  }
  return m;
}

bool appMaterializeFile( const char * filename ) {
  JNIEnv *env = appEnv;//attachThread();
  if ( env == NULL ) { return false; }
  
  ALOGV( "Materialize file %s\n", filename );
  
  jmethodID m = getMethod( env, appActivity, "copyFromApk", "(Ljava/lang/String;)Z" );
  ALOGV( "1 Materialize file %s\n", filename );
  if ( m == NULL ) { return false; }
  
  jstring s = appNewJavaString( env, filename );
  ALOGV( "2 Materialize file %s\n", filename );
  
  jboolean r = env->CallBooleanMethod( appActivity, m, s );
  ALOGV( "Materialize file %s (%s)\n", filename, r ? "succeeded" : "failed" );
  return r;
}

// ----------------------------------------------------------------------------

extern "C" {
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_setActivity(JNIEnv* env, jobject obj, jobject activity);
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_setFilesDir(JNIEnv* env, jobject obj, jstring cmd);
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_resize(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_touch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jint type, jint index);
JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_step(JNIEnv* env, jobject obj);
};

// Java interaction

Renderer* rend = nullptr;

int frame = 0;

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_setActivity(JNIEnv* env, jobject obj, jobject activity) {
  ALOGV("Activity Set");
  appActivity = env->NewGlobalRef( activity );
  appEnv = env;
}

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_setFilesDir(JNIEnv* env, jobject obj, jstring cmd) {
  appEnv = env;
  const char * str = env->GetStringUTFChars( cmd, NULL );
  ALOGV("setFilesDir: %s", str );  
  baseDir = str;
  baseDir += '/';
  env->ReleaseStringUTFChars( cmd, str );
}

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_init(JNIEnv* env, jobject obj) {  // Initialization
  appEnv = env;
  appMaterializeFile("ccol.fs");
  appMaterializeFile("ccol.vs");
  appMaterializeFile("tex.fs");
  appMaterializeFile("tex.vs");

  appMaterializeFile("0tile.png");
  appMaterializeFile("1tile.png");
  appMaterializeFile("2tile.png");
  appMaterializeFile("3tile.png");
  appMaterializeFile("4tile.png");
  appMaterializeFile("5tile.png");
  appMaterializeFile("6tile.png");
  appMaterializeFile("7tile.png");
  appMaterializeFile("8tile.png");
  appMaterializeFile("clickedMine.png");
  appMaterializeFile("flagged.png");
  appMaterializeFile("mine.png");
  appMaterializeFile("unrevealed.png");

  appMaterializeFile("Timer0.png");
  appMaterializeFile("Timer1.png");
  appMaterializeFile("Timer2.png");
  appMaterializeFile("Timer3.png");
  appMaterializeFile("Timer4.png");
  appMaterializeFile("Timer5.png");
  appMaterializeFile("Timer6.png");
  appMaterializeFile("Timer7.png");
  appMaterializeFile("Timer8.png");
  appMaterializeFile("Timer9.png");

  FILE* fp = fopen((baseDir + "ccol.fs").c_str(), "r");
  if (fp != nullptr) {
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    ALOGV("File size: %i", size);
  }

  rend = CreateRenderer();
  ALOGV("rend->Init-ing");
  rend->Init();

  fclose(fp);
}

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_resize(JNIEnv* env, jobject obj, jint jwidth, jint jheight) {  // If window shape changes
  appEnv = env;
  rend->SetWindowSize(jwidth, jheight);
}

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_touch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jint type, jint index) {  // Touch event
  ALOGV("C++ touch coords x: %f, y: %f, type: %d, index: %d", x, y, type, index);
  rend->Touch(x, y, type, index);
}

JNIEXPORT void JNICALL Java_com_android_minesweeper_MinesweeperLib_step(JNIEnv* env, jobject obj) {  // New frame
  appEnv = env;

  static int64_t count = 0;
  count++;
  float f = (count & 0xff) / float(0xff);
  glClearColor(f, f, f, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  rend->Draw(); 

  ALOGV("Frame finished %lld", count);

}
