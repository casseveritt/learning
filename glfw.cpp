#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "learning.h"
#include "linear.h"
#include "render.h"

using namespace r3;

/*
  You need dev packages to build and run this:
  sudo apt install libgles2-mesa-dev libglfw3-dev
*/

Renderer *rend = nullptr;
int frame = 0;
bool mode1;
bool drag = false;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods) {
  if(rend==nullptr) {
    return;
  }
  if (action == GLFW_PRESS) {
    frame = 0;
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_UP:
        rend->rad -= 0.25;
        break;
      case GLFW_KEY_DOWN:
        rend->rad += 0.25;
        break;
      case GLFW_KEY_LEFT:
        rend->theta -= 0.0125f;
        break;
      case GLFW_KEY_RIGHT:
        rend->theta += 0.0125f;
        break;
      case GLFW_KEY_I:
        rend->fovy -= 1.0;
        break;
      case GLFW_KEY_O:
        rend->fovy += 1.0;
        break;
      case GLFW_KEY_Z:
        if (mode1)
          mode1 = false;
        else
          mode1 = true;
        break;
      case GLFW_KEY_X:
        if (mode1)
          rend->clickRay = false;
        else
          rend->clickRay = true;
        break;
      case GLFW_KEY_C:
        rend->intersect = false;
        break;
      case GLFW_KEY_V:
        rend->iterate++;
        rend->iterate &= 1;
        break;
      case GLFW_KEY_R:
        rend->ResetSim();
        break;
      case GLFW_KEY_P:
        {
          int np = mods;//dots.numPoints + ((mods & GLFW_MOD_SHIFT) ? -1 : +1);
          printf( "cass: new numPoints=%d\n", np);
          if( np > 0 && np < 100) {
            //dots.build(np);
            //dots.reset();
          }
        }
        break;
      default:
        break;
    }
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    drag = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &rend->prevPos.x, &rend->prevPos.y);
  }
}


int main(void) {

  rend = CreateRenderer();

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  make_hints();
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(1280, 960, "Learning", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);  // This is the point when you can make gl calls
  glfwSwapInterval(1);

  rend->Init();

  while (!glfwWindowShouldClose(window)) {
    if (drag) {
      Vec2d currPos;
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      rend->diffPos = currPos - rend->prevPos;
      rend->prevPos = currPos;
      rend->theta += rend->diffPos.x * 0.0125f;
    } else {
      rend->diffPos = Vec2d();
    }

    if (mode1) {
      rend->rad += rend->diffPos.x * 0.0125f;
      rend->rad += rend->diffPos.y * 0.0125f;
    }

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    rend->SetWindowSize(width, height);

    if (rend->clickRay) {
      Vec2d currPos;
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      rend->SetCursorPos(currPos);
    }

    rend->Draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}