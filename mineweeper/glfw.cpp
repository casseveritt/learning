#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <cstring>

#include "board.h"
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
int width, height;
bool leftClick = false;
bool rightClick = false;

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
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    rightClick = (action == GLFW_PRESS);
    glfwGetCursorPos(window, &rend->prevPos.x, &rend->prevPos.y);
  }
}

/*
static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
  if (rend->intersect) {
  } else {
    rend->rad -= 0.1 * yoffset;
  }
}
*/

int main(int argc, char** argv) {
  Board board;

  int bWidth = 10;
  int bHeight = 10;
  int mines = 10;

  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);

  uint32_t seed = uint32_t(ts.tv_nsec / 10000) + uint32_t(ts.tv_sec);

  if (argc == 2) {
    seed = atoi(argv[1]);
  }
  if (argc == 4) {
    bWidth = atoi(argv[1]);
    bHeight = atoi(argv[2]);
    mines = atoi(argv[3]);
  }

  srand(seed);

  board.build(bWidth, bHeight, mines);

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
  // glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);  // This is the point when you can make gl calls
  glfwSwapInterval(1);

  rend->Init();

  bool leftClickHandled = false;
  bool rightClickHandled = false;

  while (!glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &width, &height);

    if (leftClickHandled == false && leftClick != false) {
      leftClickHandled = true;
      Vec2d p;
      glfwGetCursorPos(window, &p.x, &p.y);
      int row = board.height * (p.y / height);
      int col = board.width * (p.x / width);
      board.reveal(col, row);
    } else if (leftClickHandled != false && leftClick == false) {
      leftClickHandled = false;
    }
    if (rightClickHandled == false && rightClick != false) {
      rightClickHandled = true;
      Vec2d p;
      glfwGetCursorPos(window, &p.x, &p.y);
      int row = board.height * (p.y / height);
      int col = board.width * (p.x / width);
      board.flag(col, row);
    } else if (rightClickHandled != false && rightClick == false) {
      rightClickHandled = false;
    }

    rend->SetWindowSize(width, height);

    rend->Draw(board);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}