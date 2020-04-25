#include "glprog.h"
#include "linear.h"
#include "stb.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>

using namespace r3;

/*
  You need dev packages to build and run this:

  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

static const char *vertex_shader_text =
    "#version 300 es\n"
    "uniform highp mat4 proj;\n"
    "uniform highp mat4 view;\n"
    "uniform highp mat4 model;\n"
    "in highp vec3 pos;\n"
    "in highp vec3 col;\n"
    "out highp vec3 outcol;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = (proj * (view * (model * vec4(pos, 1.0))));\n"
    "    outcol = col;\n"
    "}\n";

static const char *fragment_shader_text = "#version 300 es\n"
                                          "in highp vec3 outcol;\n"
                                          "out mediump vec4 fragColor;\n"
                                          "void main()\n"
                                          "{\n"
                                          "    fragColor = vec4(outcol, 1.0);\n"
                                          "}\n";

float fovy = 60.0f;
Posef camPose;
Posef modelPose;
int frame = 0;
bool drag = false;
Vec2d prevPos;
Vec2d diffPos;

static void printMatrix(const Matrix4f &m) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%.2f ", m.el(i, j));
    }
    printf("\n");
  }
}

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (action == GLFW_PRESS) {
    frame = 0;
    switch (key) {
    case GLFW_KEY_ESCAPE:
      glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;
    case GLFW_KEY_W:
      modelPose.t.y += 0.025;
      break;
    case GLFW_KEY_A:
      modelPose.t.x -= 0.025;
      break;
    case GLFW_KEY_S:
      modelPose.t.y -= 0.025;
      break;
    case GLFW_KEY_D:
      modelPose.t.x += 0.025;
      break;
    case GLFW_KEY_UP:
      camPose.t.z -= 0.025;
      break;
    case GLFW_KEY_DOWN:
      camPose.t.z += 0.025;
      break;
    case GLFW_KEY_I:
      fovy -= 1.0;
      break;
    case GLFW_KEY_O:
      fovy += 1.0;
      break;
    default:
      break;
    }
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    drag = ( action == GLFW_PRESS );
    glfwGetCursorPos( window, &prevPos.x, &prevPos.y );
  } 
}

int main(void) {

  camPose.t.z = 2.0;
  GLFWwindow *window;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

  window = glfwCreateWindow(640, 480, "Learning", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window,mouse_button_callback);

  glfwMakeContextCurrent(
      window); // This is the point when you can make gl calls
  glfwSwapInterval(1);

  GLuint program = createProgram(vertex_shader_text, fragment_shader_text);

  Vec3f pos[] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
  Vec3f col[] = {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};

  GLuint vert_buffer;
  glGenBuffers(1, &vert_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(pos) + sizeof(col), nullptr,
               GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(pos), pos);
  glBufferSubData(GL_ARRAY_BUFFER, sizeof(pos), sizeof(col), col);

  GLint pos_loc = glGetAttribLocation(program, "pos");
  glVertexAttribPointer(static_cast<GLuint>(pos_loc), 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), static_cast<void *>(0));
  glEnableVertexAttribArray(static_cast<GLuint>(pos_loc));
  GLint col_loc = glGetAttribLocation(program, "col");
  glVertexAttribPointer(static_cast<GLuint>(col_loc), 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float),
                        reinterpret_cast<void *>(sizeof(pos)));
  glEnableVertexAttribArray(static_cast<GLuint>(col_loc));
  glUseProgram(program);
  GLint proj_loc = glGetUniformLocation(program, "proj");
  GLint view_loc = glGetUniformLocation(program, "view");
  GLint model_loc = glGetUniformLocation(program, "model");

  float x = 2.0;
  float y = 2.0;
  bool dir = true;
  bool lorr = true;
  int vertcol[] = {0, 1, 2};
  while (!glfwWindowShouldClose(window)) {

    if (drag) {
      Vec2d currPos;
      glfwGetCursorPos( window, &currPos.x, &currPos.y);
      diffPos = currPos - prevPos;
      prevPos = currPos;
      camPose.t.x += diffPos.x * 0.0125f;
      camPose.t.y -= diffPos.y * 0.0125f;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    float aspect = float(width) / float(height);
    Matrix4f projMat = Perspective(fovy, aspect, 0.1f, 100.0f);
    Matrix4f viewMat = camPose.Inverted().GetMatrix4();
    Matrix4f modelMat = modelPose.GetMatrix4();
    Matrix4f pvm = projMat * viewMat * modelMat;
    if (frame == 1) {
      printf("\n");
      printMatrix(pvm);
      printf("\n");
      for (int i = 0; i < 3; i++) {
        Vec3f v = pos[i];
        pvm.MultMatrixVec(v);
        printf("v: (%.2f %.2f %.2f)\n", v.x, v.y, v.z);
      }
    }
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, projMat.GetValue());
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, viewMat.GetValue());
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, modelMat.GetValue());
    glDrawArrays(GL_TRIANGLES, 0, 3);

    if (frame % 1 == 0) {

      // modelPose.t.x = 0.5 * sin(frame * 0.05f);
      // modelPose.t.y = 0.5 * cos(frame * 0.05f);
      // modelPose.r = Quaternionf(Vec3f(0, 0, 1), frame * -0.05f);

      /*Vec3f xpos[3];
      for ( int i = 0; i < 3; i++ ) {
        xpos[i] = modelPose.Transform( pos[i] );
      }
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(xpos), xpos);
      */
      /*if (dir) {
        if (lorr)
          x -= 0.0125;
        else
          x += 0.0125;
        if (x <= 0.0 || x >= 2.0) {
          dir = false;
        }
      } else {
        if (lorr)
          y -= 0.0125;
        else
          y += 0.0125;
        if (y <= 0.0 || y >= 2.0) {
          dir = true;
          lorr = !lorr;
        }
      }
      pos[2] = x - 1.0f;
      pos[5] = y - 1.0f;*/

      for (int i = 0; i < 3; i++) {
        if (vertcol[i] == 0) {
          if (col[i].x <= 0.0f)
            vertcol[i] = 1;
          else {
            col[i].x -= 0.05;
            col[i].y += 0.05;
          }
        }
        if (vertcol[i] == 1) {
          if (col[i].y <= 0.0f)
            vertcol[i] = 2;
          else {
            col[i].y -= 0.05;
            col[i].z += 0.05;
          }
        }
        if (vertcol[i] == 2) {
          if (col[i].z <= 0.0f)
            vertcol[i] = 0;
          else {
            col[i].z -= 0.05;
            col[i].x += 0.05;
          }
        }
      }
    }
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(pos), sizeof(col), col);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
