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

Renderer* rend = nullptr;
int frame = 0;
int dragTime = 0;
int width, height;
bool leftDrag = false;
bool rightDrag = false;
bool leftClick = false;
bool rightClick = false;
Vec3f nIW3;
Vec3f fIW3;
Vec2d anchor;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int mods) {
  if (rend == nullptr) {
    return;
  }
  if (action == GLFW_PRESS) {
    frame = 0;
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      /*
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
      */
      case GLFW_KEY_UP:
        rend->fovy -= 1.0;
        break;
      case GLFW_KEY_DOWN:
        rend->fovy += 1.0;
        break;
      /*
      case GLFW_KEY_Z:
        if (mode1)
          mode1 = false;
        else
          mode1 = true;
        break;
      case GLFW_KEY_X:
        clickRay = true;
        break;
      case GLFW_KEY_C:
        rend->intersect = false;
        break;
      */
      case GLFW_KEY_V:
        rend->iterate++;
        rend->iterate &= 1;
        break;
      case GLFW_KEY_R:
        rend->ResetSim();
        break;
      case GLFW_KEY_P: {
        int np = mods;  // dots.numPoints + ((mods & GLFW_MOD_SHIFT) ? -1 : +1);
        printf("cass: new numPoints=%d\n", np);
        if (np > 0 && np < 100) {
          // dots.build(np);
          // dots.reset();
        }
      } break;
      default:
        break;
    }
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    leftClick = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &rend->prevPos.x, &rend->prevPos.y);
    rend->SetCursorPos(rend->prevPos);
    rend->RayInWorld(width, height, nIW3, fIW3);
    rend->Intersect(nIW3, fIW3);  //*
    anchor = rend->prevPos;
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    rightClick = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &rend->prevPos.x, &rend->prevPos.y);
    anchor = rend->prevPos;
  }
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  if (rend->intersect) {
  } else {
    rend->rad -= 0.1 * yoffset;
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
  glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);  // This is the point when you can make gl calls
  glfwSwapInterval(1);

  rend->Init();

  while (!glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &width, &height);

    double dist;
    if (leftClick ^ rightClick) {
      if (dragTime < 8) {
        dragTime++;
      }
      Vec2d newPos;
      glfwGetCursorPos(window, &newPos.x, &newPos.y);
      double x = fabs(anchor.x - newPos.x);
      double y = fabs(anchor.y - newPos.y);
      dist = sqrt((x * x) + (y * y));
    } else {
      dragTime = 0;
      dist = 0;
    }
    leftDrag = (leftClick && (dragTime == 8 || dist >= 15));
    rightDrag = (rightClick && (dragTime == 8 || dist >= 15));

    Vec2d currPos;
    if (leftDrag || rightDrag) {
      glfwGetCursorPos(window, &currPos.x, &currPos.y);
      rend->diffPos = currPos - rend->prevPos;
      rend->prevPos = currPos;
      if (leftDrag) {
        Vec3f i;

        rend->SetCursorPos(currPos);
        rend->RayInWorld(width, height, nIW3, fIW3);

        Linef line(nIW3, fIW3);
        Planef plane(Vec3f(0, 1, 0), rend->intLoc.y);
        plane.Intersect(line, i);
        rend->Drag(i);
      }
      if (rightDrag) {
        rend->theta += rend->diffPos.x * 0.0125f;
        rend->camHeight -= rend->diffPos.y * 0.0125f;
      }
    } else {
      rend->diffPos = Vec2d();
    }

    rend->SetWindowSize(width, height);

    rend->Draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
    // if(leftDrag && dragTime==8){printf("Left dragging\n");}
    // if(rightDrag && dragTime==8){printf("Right dragging\n");}
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}