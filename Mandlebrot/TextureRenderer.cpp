/// Author: Xavier Ho (contact@xavierho.com)
#include <cstring>
#include <iostream>
#include "TextureRenderer.h"
using namespace std;

struct BBox;

TextureRenderer::TextureRenderer(int width, int height)
  : timer()
{
  this->texture_id = NULL;
  this->width = width;
  this->height = height;
  this->data = new unsigned char[width * height * 3];
  memset(this->data, 0, width * height * 3 * sizeof(unsigned char));
  glfwInit();
}

TextureRenderer::~TextureRenderer()
{
  if (data != NULL)
    delete[] data;
  glfwTerminate();
}

void TextureRenderer::start()
{
  start_threaded(1);
}

void TextureRenderer::start_threaded(int count)
{
  __start();
  thread_count = count;
  timer.start();
  threads_start(count);
  while(running) {
    render();
    handle_inputs();
    elapsed_time = timer.getMilliseconds();
    timer.start();
    cout << elapsed_time << " ms" << endl; 
  }
  threads_wait();
}

void TextureRenderer::__start()
{
  glfwOpenWindow(width, height, 8, 8, 8, 0, 8, 0, GLFW_WINDOW);
  __set_texture();
  running = true;
}

void TextureRenderer::set_window_size(int width, int height)
{
  glfwSetWindowSize(width, height);
}

void TextureRenderer::set_window_title(const char * text)
{
  glfwSetWindowTitle(text);
}

void TextureRenderer::__set_texture()
{
  glEnable(GL_TEXTURE_2D);
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,
                0, 3, width, height, 0, GL_BGR_EXT, 
                GL_UNSIGNED_BYTE, data);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1.0, 1.0);
  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 0);
  glColor3f(1, 1, 1);
}

void TextureRenderer::render()
{
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);

  glTexSubImage2D(GL_TEXTURE_2D, 
                  0, 0, 0, width, height, GL_BGR_EXT, 
                  GL_UNSIGNED_BYTE, data);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3i(0, 0, 0);
        
    glTexCoord2f(1, 0);
    glVertex3i(width, 0, 0);
        
    glTexCoord2f(1, 1);
    glVertex3i(width, height, 0);
        
    glTexCoord2f(0, 1);
    glVertex3i(0, height, 0);
  glEnd();

  glfwSwapBuffers();
  glfwPollEvents();
}

// Override this method
void TextureRenderer::handle_inputs()
{
  if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE 
          || glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
    running = false;
}