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


static const char* vertex_shader_text =
"#version 300 es\n"
"in highp vec2 pos;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(pos, 0.0, 1.0);\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 300 es\n"
"out mediump vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
"}\n";
 

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
  GLuint vertex_shader, fragment_shader, program;

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

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  float vert2d[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
  };

  GLuint vert_buffer;
  glGenBuffers( 1, &vert_buffer);
  glBindBuffer( GL_ARRAY_BUFFER, vert_buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof( vert2d ), vert2d, GL_STATIC_DRAW);

  GLint pos_loc = glGetAttribLocation(program, "pos");
  glVertexAttribPointer( static_cast<GLuint>(pos_loc), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(0));
  glEnableVertexAttribArray( static_cast<GLuint>(pos_loc) );

  int frame = 0;
  float mov = 2.0;
  bool dir = true;
  while (!glfwWindowShouldClose(window)) {
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(0.5, 20, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glDrawArrays( GL_TRIANGLES, 0, 3);

    if(frame % 5 == 0){
      if(mov == 2.0)dir = false;
      if(mov == 0.0)dir = true;
      if(dir)mov+=0.025;
      else mov-=0.025;
      vert2d[1][0] = mov-1.0f;
      vert2d[2][1] = mov-1.0f;
      glBufferSubData(GL_ARRAY_BUFFER,sizeof(vert2d),vert2d,GL_STATIC_DRAW);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
