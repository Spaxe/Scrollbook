/// Author: Xavier Ho (contact@xavierho.com)
#ifdef _WIN32
  #include "../GL/glew.h"
  #include "../GL/glfw.h"
#else
  #include <GL/glew.h>
  #include <GL/glfw.h>
#endif
#include <cstring>
#include <iostream>
#include "TextureRenderer.h"
using namespace std;

TextureRenderer::TextureRenderer()
{
  data = NULL;
  texture_id = NULL;
  glfwInit();
  glfwSetWindowTitle("Render View"); // TODO
}

TextureRenderer::TextureRenderer(int width, 
                                 int height, 
                                 unsigned char * data)
{
  this->data = NULL;
  this->texture_id = NULL;
  setTexture(width, height, data);
  setWindowSize(width, height);
  glfwInit();
  glfwSetWindowTitle("Render View"); // TODO
}

TextureRenderer::~TextureRenderer()
{
  if (data != NULL)
    delete[] data;
  glfwTerminate();
}

pthread_t TextureRenderer::start()
{
  running = true;
  glfwOpenWindow(img_width, img_height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);
  pthread_t id;
  int r = pthread_create(&id, NULL, &TextureRenderer::mainloop_threaded, this);
  if (r)
    cerr << "Unable to start TextureRenderer: " << r << endl;
  return id;
}

void TextureRenderer::stop(pthread_t id)
{
  void * status;
  running = false;
  int r = pthread_join(id, &status);
  if (r)
     cerr << "TextureRenderer thread failed to join: " << r  << endl;
  glfwCloseWindow();
}

void TextureRenderer::setWindowSize(int width, int height)
{
  glfwSetWindowSize(width, height);
}

void TextureRenderer::setTexture(int width, int height, unsigned char * data)
{
  if (this->data != NULL) {
    has_texture = false;
    delete[] this->data;
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture_id);
  }

  this->img_width = width;
  this->img_height = height;
  this->data = new unsigned char[width * height * 3];
  memcpy(this->data, data, width * height * 3 * sizeof(unsigned char));
  
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,
                0, 3, width, height, 0, GL_BGR_EXT, 
                GL_UNSIGNED_BYTE, data);
  has_texture = true;
}

void TextureRenderer::setTexture(unsigned char * data)
{  
  glTexSubImage2D(GL_TEXTURE_2D, 
                  0, 0, 0, img_width, img_height, GL_BGR_EXT, 
                  GL_UNSIGNED_BYTE, data);
}

void * TextureRenderer::mainloop_threaded(void * obj)
{
  ((TextureRenderer *) obj)->mainloop();
  cout << "TextureRenderer child thread exiting..." << endl;
  pthread_exit(0);
  return NULL;
}

void TextureRenderer::mainloop()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, img_width, 0, img_height, -1.0, 1.0);
  glViewport(0, 0, img_width, img_height);
  glClearColor(0, 0, 0, 0);
  glEnable(GL_TEXTURE_2D);

  while(running) {
    calculate();    
    render();
    handleInputs();
    cout << "Thread running." << endl;
  }
}

void TextureRenderer::render()
{
  while (!has_texture) {
  } // Block if texture is loading
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3i(0, 0, 0);
        
    glTexCoord2f(1, 0);
    glVertex3i(img_width, 0, 0);
        
    glTexCoord2f(1, 1);
    glVertex3i(img_width, img_height, 0);
        
    glTexCoord2f(0, 1);
    glVertex3i(0, img_height, 0);
  glEnd();

  glfwSwapBuffers();
}

void TextureRenderer::calculate()
{
  // Override this method
}

void TextureRenderer::handleInputs()
{
  // Override this method
}

/* Testing */
int main()
{
  unsigned char d[256 * 256 * 3];
  for (int i = 0; i < 256 * 256; i+=3) {
    d[i] = 128;
    d[i+1] = 128;
    d[i+2] = 128;
  }
  TextureRenderer renderer(256, 256, d);
  pthread_t id = renderer.start();
  int i;
  cin >> i;
  renderer.stop(id);
  return 0;
}