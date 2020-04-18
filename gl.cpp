#include "stb.h"
#include <stdio.h>
#include <GLES3/gl32.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>

/*
  You need dev packages to build and run this:

  sudo apt install libgles2-mesa-dev libglfw3-dev

*/
static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
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

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  int frame = 0;
  while (!glfwWindowShouldClose(window)) {
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(1, 20, (frame & 255) / 155.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
