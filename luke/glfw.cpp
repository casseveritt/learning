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
int frame = 0, dragTime = 0;
bool leftDrag = false, rightDrag = false;
bool leftClick = false, rightClick = false;
Vec2d anchor;  // Anchored location on the screen from inital click for drag functionality
float rad = 2.5;
Vec3f cameraLocation;
Vec2d diffPos, prevPos, currPos;

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods) {
  if (rend == nullptr) {
    return;
  }
  if (action == GLFW_PRESS || action == GLFW_REPEAT) {
    // frame = 0;
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;
      case GLFW_KEY_UP:
        rend->fovy -= 1.0;
        break;
      case GLFW_KEY_DOWN:
        rend->fovy += 1.0;
        break;
      case GLFW_KEY_W:
        break;
      case GLFW_KEY_A:
        break;
      case GLFW_KEY_S:
        break;
      case GLFW_KEY_D:
        break;

      case GLFW_KEY_V:  // Need to check why this breaks if left on
        rend->iterate++;
        rend->iterate &= 1;
        break;
      case GLFW_KEY_R:
        rend->ResetSim();
        break;
      default:
        break;
    }
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      leftClick = true;
      glfwGetCursorPos(window, &prevPos.x, &prevPos.y);
      rend->SetCursorPos(prevPos);
      rend->Intersect();
      anchor = prevPos;
    } else if (action == GLFW_RELEASE) {
      leftClick = false;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      rightClick = (action == GLFW_PRESS);
      glfwGetCursorPos(window, &prevPos.x, &prevPos.y);
      anchor = prevPos;
    } else if (action == GLFW_RELEASE) {
      rightClick = false;
    }
  }
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
    rend->trackCamera = !rend->trackCamera;
  }
}

static void scroll_callback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
  if (rend->trackCamera) rad -= 0.1 * yoffset;
}

int main(void) {
  rend = CreateRenderer();
  rend->width = 1280;
  rend->height = 960;
  rend->camera.t.z = rad;

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  make_hints();
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(rend->width, rend->height, "Learning", NULL, NULL);
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

  float theta;

  while (!glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &rend->width, &rend->height);

    double dist;
    if (leftClick ^ rightClick) {  // Click functionality (caret^ is boolean XOR gate)
      if (dragTime < 8) {          // Checking if click is a drag
        dragTime++;
      }
      Vec2d newPos;
      glfwGetCursorPos(window, &newPos.x, &newPos.y);
      double x = fabs(anchor.x - newPos.x);
      double y = fabs(anchor.y - newPos.y);
      dist = sqrt((x * x) + (y * y));
    } else {  // Release of click/drag
      dragTime = 0;
      dist = 0;
    }
    leftDrag = (leftClick && (dragTime == 8 || dist >= 15));    // Determines if there is a left click drag
    rightDrag = (rightClick && (dragTime == 8 || dist >= 15));  // Determines if there is a right click drag

    glfwGetCursorPos(window, &currPos.x, &currPos.y);
    diffPos = currPos - prevPos;
    prevPos = currPos;

    if (leftDrag || rightDrag) {  // Drag functionality
      if (leftDrag && rend->intersect) {
        Vec3f i;
        Vec3f nIW3, fIW3;

        rend->SetCursorPos(currPos);
        rend->RayInWorld(nIW3, fIW3);

        Linef line(nIW3, fIW3);
        Planef plane(Vec3f(0, 1, 0), rend->intLoc.y);
        plane.Intersect(line, i);
        rend->Drag(i);
      }
      if (rightDrag && rend->trackCamera) {
        theta += diffPos.x * 0.0125f;
        rend->camera.t.x = sin(theta) * rad;
        rend->camera.t.z = cos(theta) * rad;
        rend->camera.t.y -= diffPos.y * 0.0125f;
        rend->camera.r.SetValue(Vec3f(0, 0, -1), Vec3f(0, 1, 0), -rend->camera.t, Vec3f(0, 1, 0));
      }
    } else if (!rend->trackCamera) {
      Quaternionf camRotX(Vec3f(1, 0, 0), (-diffPos.y * 0.0125f));
      Quaternionf camRotY(Vec3f(0, 1, 0), (-diffPos.x * 0.0125f));
      rend->camera.r *= camRotY;
      rend->camera.r *= camRotX;
    } else {
      diffPos = Vec2d();
    }

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