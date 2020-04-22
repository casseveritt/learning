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
"in highp vec3 col;\n"
"out highp vec3 outcol;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(pos, 0.0, 1.0);\n"
"    outcol = col;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 300 es\n"
"in highp vec3 outcol;\n"
"out mediump vec4 fragColor;\n"
"void main()\n"
"{\n"
"    fragColor = vec4(outcol, 1.0);\n"
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

  float pos[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f
  };
  float col[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
  };

  GLuint vert_buffer;
  glGenBuffers( 1, &vert_buffer);
  glBindBuffer( GL_ARRAY_BUFFER, vert_buffer );
  glBufferData( GL_ARRAY_BUFFER, sizeof(pos)+sizeof(col), nullptr, GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(pos),pos);
  glBufferSubData(GL_ARRAY_BUFFER,sizeof(pos),sizeof(col),col);

  GLint pos_loc = glGetAttribLocation(program, "pos");
  glVertexAttribPointer( static_cast<GLuint>(pos_loc), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void*>(0));
  glEnableVertexAttribArray( static_cast<GLuint>(pos_loc) );
  GLint col_loc = glGetAttribLocation(program, "col");
  glVertexAttribPointer( static_cast<GLuint>(col_loc), 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(sizeof(pos)));
  glEnableVertexAttribArray( static_cast<GLuint>(col_loc) );

  int frame = 0;
  float x = 2.0;
  float y = 2.0;
  bool dir = true;
  bool lorr = true;
  int vertcol[] = {0,1,2};
  while (!glfwWindowShouldClose(window)) {
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glClearColor(1.0f,1.0f,1.0f, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(program);
    glDrawArrays( GL_TRIANGLES, 0, 3);

    if(frame % 1 == 0){
      if(dir){
        if(lorr)x-=0.0125;
        else x+=0.0125;
        if(x<=0.0 || x>=2.0){
          dir=false;
        }
      }else{
        if(lorr)y-=0.0125;
        else y+=0.0125;
        if(y<=0.0 || y>=2.0){
          dir=true;
          lorr = !lorr;
        }
      }
      pos[2] = x-1.0f;
      pos[5] = y-1.0f;
    
      for(int i=0;i<3;i++){
        if(vertcol[i]==0){
          if(col[(i*3)]<=0.0f)vertcol[i]=1;
          else{
            col[(i*3)]-=0.025;
            col[(i*3)+1]+=0.025;
          }
        }if(vertcol[i]==1){
          if(col[(i*3+1)]<=0.0f)vertcol[i]=2;
          else{
            col[(i*3)+1]-=0.025;
            col[(i*3)+2]+=0.025;
          }
        }if(vertcol[i]==2){
          if(col[(i*3)+2]<=0.0f)vertcol[i]=0;
          else{
            col[(i*3)+2]-=0.025;
            col[(i*3)]+=0.025;
          }
        }
      }
    }
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(pos),pos);
    glBufferSubData(GL_ARRAY_BUFFER,sizeof(pos),sizeof(col),col);

    glfwSwapBuffers(window);
    glfwPollEvents();
    frame++;
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
