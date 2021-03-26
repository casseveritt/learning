#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <atomic>
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
int frame = 0, width, height;
std::atomic<uint32_t> leftClick(0);
std::atomic<uint32_t> rightClick(0);

Board board;
int bWidth = 10, bHeight = 10, mines = 10;

bool MSARunning = false; // Minesweeper Algorithm
double t0;

static double getTimeInSeconds() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return double(int64_t(ts.tv_sec) * int64_t(1e9) + int64_t(ts.tv_nsec)) * 1e-9;
}

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
      case GLFW_KEY_V:
        MSARunning = !MSARunning;
        t0 = getTimeInSeconds();
        break;
      default:
        break;
    }
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int /*mods*/) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action != GLFW_PRESS) {
      leftClick++;
      // printf("left click = %d\n", leftClick.load());
    }
    glfwGetCursorPos(window, &rend->prevPos.x, &rend->prevPos.y);
    rend->SetCursorPos(rend->prevPos);
  }
  if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    if (action == GLFW_PRESS) {
      rightClick++;
      // printf("right click = %d\n", rightClick.load());
    }
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

  // seed = 0;
  srand(seed);

  // printf("w=%d, h=%d, mines=%d\n", bWidth, bHeight, mines);

  rend = CreateRenderer();

  board.build(bWidth, bHeight, mines);

  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  make_hints();
  glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

  GLFWwindow* window = glfwCreateWindow(2560, 2560, "Learning", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  // glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);  // This is the point when you can make gl calls
  glfwSwapInterval(1);

  rend->Init(board);

  uint32_t prevLeftClick = 0;
  uint32_t prevRightClick = 0;

  while (!glfwWindowShouldClose(window)) {
    glfwGetFramebufferSize(window, &width, &height);

    if (prevLeftClick < leftClick.load()) {
      prevLeftClick = leftClick.load();
      // printf("leftClick = %d\n", prevLeftClick);
      Vec2d p;
      glfwGetCursorPos(window, &p.x, &p.y);
      int row = board.height * (p.y / height);
      int col = board.width * (p.x / width);
      board.reveal(col, row);
    }

    if (prevRightClick < rightClick.load()) {
      prevRightClick = rightClick.load();
      // printf("rightClick = %d\n", prevRightClick);
      Vec2d p;
      glfwGetCursorPos(window, &p.x, &p.y);
      int row = board.height * (p.y / height);
      int col = board.width * (p.x / width);
      board.flag(col, row);
    }

    if (MSARunning && t0 >= 0.5) {
      if (Board::State == Uninitalized) {
        board.reveal(rand()%bWidth, rand()%bHeight);
      } else if (Board::State == Playing) {
        std::vector<float> probability;
        for (Board::Tile t : board.board) {
          
        }
      }
      t0 = getTimeInSeconds();

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