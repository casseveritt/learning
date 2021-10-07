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
#include "gles3jni.h"
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
bool dragdetect = false;

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
  Space_Window,
  Space_Board,
  Space_Tile
};

struct Transforms {

  Transforms() {
    xfScreenFromPixel = Matrix4f::Identity();
    auto r = Matrix4f::Translate(Vec3f(-0.5f, -0.5f, 0.0f));
    r = Quaternionf(Vec3f(0.0f, 0.0f, -1.0f), ToRadians(45.0f)).GetMatrix4() * r;
    r = Matrix4f::Translate(Vec3f(0.5f, 0.5f, 0.0f)) * r;

    auto s = Matrix4f::Scale(Vec3f(1.2f, 1.2f, 1));
    auto t = Matrix4f::Translate(Vec3f(-0.1f, -0.5f, 0));
    xfBoardFromScreen = r * t * s;
    xfBoardTileFromBoard = Matrix4f::Identity();
    xfBoardFromBoardWindow = Matrix4f::Identity();
  }
  void SetPixels(int w, int h) {
    auto s = Matrix4f::Scale(Vec3f((1.0f/w), -(1.0f/w),1));
    auto t = Matrix4f::Translate(Vec3f(0,(float(h)/w),0));
    xfScreenFromPixel = t * s;
  }
  void SetTiles(int r, int c) {
    xfBoardTileFromBoard.SetScale(Vec3f(float(c),float(r),1));
  }
  Matrix4f transform(Space to, Space from) {
    if (from == Space_Tile && to == Space_Pixel) {
      return (xfBoardTileFromBoard * xfBoardFromScreen * xfScreenFromPixel).Inverted();
    }
    if (from == Space_Tile && to == Space_Window) {
      return (xfBoardTileFromBoard * xfBoardFromScreen).Inverted();
    }
    if (from == Space_Board && to == Space_Pixel) {
      return (xfBoardFromScreen * xfScreenFromPixel).Inverted();
    }
    if (from == Space_Pixel && to == Space_Window) {
      return xfScreenFromPixel;
    }
    if (from == Space_Pixel && to == Space_Tile) {
      return (xfBoardTileFromBoard * xfBoardFromScreen * xfScreenFromPixel);
    }
    ALOGE("Transforms::transform unsupported transform");
    exit(42);
    return Matrix4f();
  }

  Matrix4f xfScreenFromPixel;
  Matrix4f xfBoardFromScreen;
  Matrix4f xfBoardTileFromBoard;
  Matrix4f xfBoardFromBoardWindow;
};

Transforms xf;

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
  Rectangle testrect;

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

void RendererImpl::Init(const Board& b) {


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

  ALOGV("Object building");
  xf.SetTiles(b.width, b.height);

  tiles.s0 = 1.0f;
  tiles.s1 = 1.0f;
  //tiles.obj.modelPose = Posef();
  //tiles.obj.model = xf.transform(Space_Pixel, Space_Board);
  //tiles.obj.model = Matrix4f::Identity();

  testrect.build(1.0f,1.0f,Vec3f(1.0f,0.0f,1.0f));

  ALOGV("Done initializing");
}

/*
static void mouse_button_callback(int button, int action) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      leftClick = true;
      anchor = prevPos;
    } else if (action == GLFW_RELEASE) {
      leftClick = false;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      rightClick = (action == GLFW_PRESS);
      anchor = prevPos;
    } else if (action == GLFW_RELEASE) {
      rightClick = false;
    }
  }
}
*/

void RendererImpl::SetWindowSize(int w, int h) {
  width = w;
  height = h;
  xf.SetPixels(w, h);
  ALOGV("Screen Dimensions: Width = %d, Height = %d", w, h);
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

  scene.camPose.t.SetValue(0,0,1);
  scene.camPose.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), Vec3f(0, 0, -1), Vec3f(0, 1, 0));

  tiles.obj.model = xf.transform(Space_Window, Space_Tile);

  //glViewport(0, 0, width, height);

  //glClearColor(0.05f, 0.05f, 0.05f, 0);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  float aspectRatio = float(height) / width;
  scene.projMat = Ortho(0.0f, 1.0f, 0.0f, float(height)/width, -1.0f, 1.0f);

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

  if (dragdetect) {
    testrect.draw(scene, constColorProg);
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
  

  //testrect.draw(scene, constColorProg);

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
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_setActivity(JNIEnv* env, jobject obj, jobject activity);
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_setFilesDir(JNIEnv* env, jobject obj, jstring cmd);
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_init(JNIEnv* env, jobject obj);
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint width, jint height);
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_touch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jint type);
JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_step(JNIEnv* env, jobject obj);
};

// Java interaction

Renderer* rend = nullptr;

Board board;
int frame = 0, width, height;
bool held = false;
int framesHeld = 0;
int bWidth = 10, bHeight = 10, mines = 10;

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_setActivity(JNIEnv* env, jobject obj, jobject activity) {
  ALOGV("Activity Set");
  appActivity = env->NewGlobalRef( activity );
  appEnv = env;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_setFilesDir(JNIEnv* env, jobject obj, jstring cmd) {
  appEnv = env;
  const char * str = env->GetStringUTFChars( cmd, NULL );
  ALOGV("setFilesDir: %s", str );  
  baseDir = str;
  baseDir += '/';
  env->ReleaseStringUTFChars( cmd, str );
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_init(JNIEnv* env, jobject obj) {  // Initialization
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

  FILE* fp = fopen((baseDir + "ccol.fs").c_str(), "r");
  if (fp != nullptr) {
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    ALOGV("File size: %i", size);
  }

  srand(0); // Random board seed
  rend = CreateRenderer();
  board.build(bWidth, bHeight, mines);
  ALOGV("rend->Init-ing");
  rend->Init(board);

  fclose(fp);
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_resize(JNIEnv* env, jobject obj, jint jwidth, jint jheight) {  // If window shape changes
  appEnv = env;
  rend->SetWindowSize(jwidth, jheight);
  width = jwidth;
  height = jheight;
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_touch(JNIEnv* env, jobject obj, jfloat x, jfloat y, jint type) {  // Touch event
  ALOGV("C++ touch coords x: %f, y: %f", x, y);
  Vec3f posInPixels(x, y, 1.0f);
  if (type == 262) {
    dragdetect = true;
  }
  if (type == 1) {
    Vec3f posInTiles = xf.transform(Space_Tile, Space_Pixel) * posInPixels;
    if (held) {
      ALOGV("Revealing tile x: %f, y: %f", posInTiles.x, posInTiles.y);
      board.reveal(posInTiles.x, posInTiles.y);
      held = false;
    } else {
      board.flag(posInTiles.x, posInTiles.y);
    }
    framesHeld = 0;
  } if (type == 0 || type == 2) {
    framesHeld++;
    if (framesHeld >= 10) {
      held = true;
    }
  }
}

JNIEXPORT void JNICALL Java_com_android_gles3jni_GLES3JNILib_step(JNIEnv* env, jobject obj) {  // New frame
  appEnv = env;

  static int64_t count = 0;
  count++;
  float f = (count & 0xff) / float(0xff);
  glClearColor(f, f, f, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  rend->Draw(board); 

  ALOGV("Frame finished %d", count);

}
