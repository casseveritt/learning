#include "learning.h"
#include "glprog.h"
#include <stdio.h>
#include "linear.h"
#include "stb.h"
#include <math.h>
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
Vec3f lightPos(0.0f,1.0f,0.0f);

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow *window, int key, int /*scancode*/, int action,
                         int /*mods*/) {
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

class Object {

public:
  GLuint b;
  GLint pos_loc, col_loc, proj_loc, view_loc, model_loc, norm_loc;
  Posef modelPose;
  GLenum primType;
  struct Vertex {
    Vec3f color;
    Vec3f normal;
    Vec3f position;
  };
  Vertex v;
  std::vector<Vertex> verts;

  void begin(GLenum prim) {
    glGenBuffers(1, &b);
    primType = prim;
    verts.clear();
  }

  void end() {
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, float(sizeof(Vertex) * verts.size()),
                 &verts[0], GL_DYNAMIC_DRAW);
  }

  void color(float r, float g, float bl) { v.color = Vec3f(r, g, bl); }

  void normal(float x, float y, float z) { v.normal = Vec3f(x, y, z); }

  void normal(Vec3f norm) { v.normal = norm; }

  void position(float x, float y, float z) {
    v.position = Vec3f(x, y, z);
    verts.push_back(v);
  }

  void position( Vec3f cords ) {
    v.position = cords;
    verts.push_back(v);
  }

  void draw(Prog p) {
    glUseProgram(p.p);
    Matrix4f viewMat = camPose.Inverted().GetMatrix4();
    glBindBuffer(GL_ARRAY_BUFFER, b);
    if(p.col.i >= 0){
      glVertexAttribPointer(p.col.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(0));
      glEnableVertexAttribArray(p.col.u);
    }if(p.norm.i >= 0){
      glVertexAttribPointer(p.norm.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(sizeof(Vec3f)));
      glEnableVertexAttribArray(p.norm.u);
    }if(p.pos.i >= 0){
      glVertexAttribPointer(p.pos.u, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), OFFSET_OF(sizeof(Vec3f)*2));
      glEnableVertexAttribArray(p.pos.u);
    }

    Matrix4f modelMat = modelPose.GetMatrix4();
    glUniformMatrix4fv(p.proj.i, 1, GL_FALSE, projMat.GetValue());
    glUniformMatrix4fv(p.view.i, 1, GL_FALSE, viewMat.GetValue());
    glUniformMatrix4fv(p.model.i, 1, GL_FALSE, modelMat.GetValue());
    glUniform3fv(p.lightPos.i, 1, &lightPos.x);
    glDrawArrays(primType, 0, verts.size());

    if(p.col.i >= 0)  glDisableVertexAttribArray(p.pos.u);
    if(p.norm.i >= 0) glDisableVertexAttribArray(p.norm.u);
    if(p.pos.i >= 0)  glDisableVertexAttribArray(p.col.u);
    glBindBuffer(GL_ARRAY_BUFFER, dummy_buffer);
    glUseProgram(dummy_program);
  }
};

class Cube { // 0,1,2 - 1,2,3 // 2,3,4 - 3,4,5 // 4,5,6 - 5,6,7 // 6,7,0 - 7,0,1 // 6,0,2 - 0,4,2 // 0,1,4 - 1,4,5

public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Vert cubeVerts[8];
  Object cubePolys[12];

  void begin(float x, float y, float z, float s = 1.0){
    for(int i=0;i<8;i++) cubeVerts[i].col = Vec3f(0.0f, 0.0f, 0.0f);
    cubeVerts[0].pos = Vec3f((0.0f*s) + x, (0.0f*s) + y, (0.0f*s) + z);
    cubeVerts[1].pos = Vec3f((1.0f*s) + x, (0.0f*s) + y, (0.0f*s) + z);
    cubeVerts[2].pos = Vec3f((0.0f*s) + x, (1.0f*s) + y, (0.0f*s) + z);
    cubeVerts[3].pos = Vec3f((1.0f*s) + x, (1.0f*s) + y, (0.0f*s) + z);
    cubeVerts[6].pos = Vec3f((0.0f*s) + x, (0.0f*s) + y, (-1.0f*s) + z);
    cubeVerts[7].pos = Vec3f((1.0f*s) + x, (0.0f*s) + y, (-1.0f*s) + z);
    cubeVerts[4].pos = Vec3f((0.0f*s) + x, (1.0f*s) + y, (-1.0f*s) + z);
    cubeVerts[5].pos = Vec3f((1.0f*s) + x, (1.0f*s) + y, (-1.0f*s) + z);

    //

    for(int i=0;i<8;i++){
      cubePolys[i].begin(GL_TRIANGLES);
      cubePolys[i].color(0.0f,0.0f,0.0f);
      for(int j=0;j<3;j++){
        int l = (i+j) % 8; // Does i, i+1, and i+2 verts (Loops around to 0 if over 7)
        cubePolys[i].position(cubeVerts[l].pos);
      }
      cubePolys[i].end();
    }
    cubePolys[8].begin(GL_TRIANGLES);
    cubePolys[8].color(0.0f,0.0f,0.0f);
    cubePolys[8].position(cubeVerts[2].pos);
    cubePolys[8].position(cubeVerts[4].pos);
    cubePolys[8].position(cubeVerts[0].pos);
    cubePolys[8].end();
    cubePolys[9].begin(GL_TRIANGLES);
    cubePolys[9].color(0.0f,0.0f,0.0f);
    cubePolys[9].position(cubeVerts[4].pos);
    cubePolys[9].position(cubeVerts[6].pos);
    cubePolys[9].position(cubeVerts[0].pos);
    cubePolys[9].end();

    cubePolys[10].begin(GL_TRIANGLES);
    cubePolys[10].color(0.0f,0.0f,0.0f);
    cubePolys[10].position(cubeVerts[3].pos);
    cubePolys[10].position(cubeVerts[5].pos);
    cubePolys[10].position(cubeVerts[1].pos);
    cubePolys[10].end();
    cubePolys[11].begin(GL_TRIANGLES);
    cubePolys[11].color(0.0f,0.0f,0.0f);
    cubePolys[11].position(cubeVerts[5].pos);
    cubePolys[11].position(cubeVerts[7].pos);
    cubePolys[11].position(cubeVerts[1].pos);
    cubePolys[11].end();
  }

  void draw(Prog p){
    for(int i=0;i<12;i++) cubePolys[i].draw(p);
  }
};

class Sphere {

public:
  struct Vert {
    Vec3f col;
    Vec3f norm;
    Vec3f pos;
  };
  Object sphObj;

  void begin(float x, float y, float z, float radi = 0.5){
    Vec3f center = Vec3f(x,y+radi,z);
    std::vector<Vec3f> circle;
    for(int i=0;i<37;i++){ // Degrees
      float tr = ToRadians(i * 10.0f);
      circle.push_back(Vec3f(sin(tr)*radi,cos(tr)*radi+radi+y,0.0));
      theta += 10.0;
    }

    sphObj.begin(GL_TRIANGLES);
    sphObj.color(0.0f,0.0f,0.0f);
    for(int j=0;j<18;j++){
      Quaternionf q0(Vec3f( 0, 1, 0 ), ToRadians( j*10.0f ));
      Quaternionf q1(Vec3f( 0, 1, 0 ), ToRadians( (j*10.0f)+10.0f ));
      for(size_t i=0;i<circle.size()-1;i++){
        Vec3f v00 = q0 * circle[i+0] + Vec3f(x,y,z);
        Vec3f v01 = q1 * circle[i+0] + Vec3f(x,y,z);
        Vec3f v10 = q0 * circle[i+1] + Vec3f(x,y,z);
        Vec3f v11 = q1 * circle[i+1] + Vec3f(x,y,z);
        sphObj.normal((v00-center).Normalized());
        sphObj.position(v00);
        sphObj.normal((v10-center).Normalized());
        sphObj.position(v10);
        sphObj.normal((v01-center).Normalized());
        sphObj.position(v01);

        sphObj.normal((v01-center).Normalized());
        sphObj.position(v01);
        sphObj.normal((v10-center).Normalized());
        sphObj.position(v10);
        sphObj.normal((v11-center).Normalized());
        sphObj.position(v11);
      }
    }
    sphObj.end();
  }

  void draw(Prog p){
    sphObj.draw(p);
  }
};

class Torus {

public:
  struct Vert {
    Vec3f col;
    Vec3f pos;
  };
  Object torObj;

  void begin(float rad1 = 0.5, float rad2 = 0.25){
    std::vector<Vec3f> torus;
    for(int i=0;i<37;i++){ // Degrees
      float tr = ToRadians(i * 10.0f);
      torus.push_back(Vec3f(sin(tr)*rad2+rad1,cos(tr)*rad2,0.0));
      theta += 10.0;
    }

    Vec3f center(rad1, 0, 0);
    torObj.begin(GL_TRIANGLES);
    torObj.color(0.0f,0.0f,0.0f);
    for(int j=0;j<36;j++){
      Quaternionf q0(Vec3f( 0, 1, 0 ), ToRadians( j*10.0f ));
      Quaternionf q1(Vec3f( 0, 1, 0 ), ToRadians( (j*10.0f)+10.0f ));
      for(size_t i=0;i<torus.size()-1;i++){
        Vec3f v00 = q0 * torus[i+0];
        Vec3f v01 = q1 * torus[i+0];
        Vec3f v10 = q0 * torus[i+1];
        Vec3f v11 = q1 * torus[i+1];
        Vec3f c0 = q0 * center;
        Vec3f c1 = q1 * center;
        torObj.normal((v00-c0).Normalized());
        torObj.position(v00);
        torObj.normal((v10-c0).Normalized());
        torObj.position(v10);
        torObj.normal((v01-c1).Normalized());
        torObj.position(v01);

        torObj.normal((v01-c1).Normalized());
        torObj.position(v01);
        torObj.normal((v10-c0).Normalized());
        torObj.position(v10);
        torObj.normal((v11-c1).Normalized());
        torObj.position(v11);
      }
    }
    torObj.end();
  }

  void draw(Prog p){
    torObj.draw(p);
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

  GLint version_maj=0;
  GLint version_min=0;
  glGetIntegerv( GL_MAJOR_VERSION, &version_maj );
  glGetIntegerv( GL_MINOR_VERSION, &version_min );
  printf( "GL Version: %d.%d\n", version_maj, version_min );

  GLuint defaultVab;
  glGenVertexArrays(1, &defaultVab);
  glBindVertexArray( defaultVab );

  // programs init begin
  GLuint prog = createProgram("progs/Vertex-Shader.vs", "progs/Fragment-Shader.fs");
  glUseProgram(prog);
  Prog program;
  program.set(prog);

  GLuint litProg = createProgram("progs/Lit-Vertex.vs", "progs/Lit-Fragment.fs");
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
  cub.begin(-1.0f,0.0f,-1.0f,0.75f);

  Object cube;
  makeCube( cube, Matrix4f::Scale(0.375f) );
  cube.modelPose.t = Vec3f(-1,0.375f,-1);

  Sphere sph;
  sph.begin(1.0f,0.0f,-1.0f,0.5f);

  Torus tor;
  tor.torObj.modelPose.t = Vec3f( 1, 0.25f, 1);
  tor.begin(0.5f,0.25f);

  Sphere light;
  light.begin(0.0f, 0.0f,0.0f, 0.0525f);
  // objects init end

  while (!glfwWindowShouldClose(window)) {

    if(drag){
      Vec2d currPos;
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      diffPos = currPos - prevPos;
      prevPos = currPos;
      theta += diffPos.x * 0.0125f;
    }else{
      diffPos = Vec2d();
    }

    if(mode1){
      rad += diffPos.x * 0.0125f;
      rad += diffPos.y * 0.0125f;
    }else{
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
    //cub.draw(program);
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
