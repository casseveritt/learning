#include "glprog.h"

#include "stb.h"
#include <GLES3/gl32.h>
#include <stdio.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>

GLuint createProgram(const char *vertexShader, const char *fragmentShader) {
  GLuint vertex_shader, fragment_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertexShader, NULL);
  glCompileShader(vertex_shader);
  GLint stat = GL_FALSE;
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Vertex comple failed\n");
  }
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragmentShader, NULL);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &stat);
  if (stat == GL_FALSE) {
    printf("Fragment comple failed\n");
  }

  GLuint program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  return program;
}