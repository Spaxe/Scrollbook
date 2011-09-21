///
/// Author: Xavier Ho (contact@xavierho.com)
///
#include <iostream>
#include <cmath>
#include <string>
#include <string.h>
#include "GL\glew.h"
#include "GL\glfw.h"
#include "Timer.h"
#include "Mandlebrot.h"
using namespace std;

#define CUDA_ERROR(x) \
{ \
  cudaError_t r = x; \
  if (r != cudaSuccess) \
    { cout << #x << "\n" << r << ": " << cudaGetErrorString(r) << endl; } \
}

/// Basic implementation of Mandlebrot on CUDA
__global__ void mandlebrot(unsigned char * data, int width, int height, float tx, float ty, float scale, int limit)
{
  // Convert to normalised texture coordinates
  unsigned int u = blockIdx.x * blockDim.x + threadIdx.x;
  unsigned int v = blockIdx.y * blockDim.y + threadIdx.y;

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      unsigned int addr = v*width*3*4 + u*3*4 + i*width*3 + j*3;
      float cr = float(u*4 + j + tx) / width * scale;
      float ci = float(v*4 + i + ty) / height * scale;

      int c = 0;
      float tmp, x = 0, y = 0;
      while ((x*x + y*y < 4) && (c++ < limit)) {
          tmp = x*x - y*y + cr;
          y = 2 * x * y + ci;
          x = tmp;
      }
      if (c >= limit) {
        data[addr]   =  (unsigned char) 0;
        data[addr+1] =  (unsigned char) 0;
        data[addr+2] =  (unsigned char) 0;
      } else {
        unsigned char value = (unsigned char)((float)c / limit * 255);
        data[addr]   = value;
        data[addr+1] = value / 2;
        data[addr+2] = value / 4;
      }
    }
  }
}


Mandlebrot::Mandlebrot(int width, int height)
{
  this->width = width;
  this->height = height;
  this->limit = 64;
  this->scale = 3.0f;
  this->tx = -width * 5 / 7.0f;
  this->ty = -height / 2.0f;
  this->data = new unsigned char[width * height * 3];
  memset(this->data, 0, sizeof(data));
  this->size = width * height * 3 * sizeof(unsigned char);
  CUDA_ERROR(cudaMalloc((void **)&this->gpu_data, this->size));
}

Mandlebrot::~Mandlebrot()
{
  delete[] this->data;
  cudaFree(gpu_data);
}

void Mandlebrot::render()
{
  glfwInit();
  glfwOpenWindow(width, height, 8, 8, 8, 0, 8, 0, GLFW_WINDOW);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1.0, 1.0);
  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 0);
  glColor3f(1, 1, 1);

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

  CUDA_ERROR(cudaMemcpy(gpu_data, data, this->size, cudaMemcpyHostToDevice));

  timer.start();
  while (running)
  {
    dim3 threadsPerBlock(16, 16);
    dim3 numBlocks(width/4/threadsPerBlock.x, height/4/threadsPerBlock.y);
    mandlebrot<<<numBlocks, threadsPerBlock>>>(gpu_data, width, height, tx, ty, scale, limit);
    CUDA_ERROR(cudaThreadSynchronize());
    CUDA_ERROR(cudaMemcpy(data, gpu_data, this->size, cudaMemcpyDeviceToHost));

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
    handle_inputs();
    elapsed_time = timer.getMilliseconds();
    cout << elapsed_time << " ms" << endl;
    timer.start();
  }
}

void Mandlebrot::handle_inputs()
{
  if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE 
          || glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
    running = false;
  if (glfwGetKey('H') == GLFW_PRESS) {
    scale = 2.f;
    tx = -width * 2 / 3.0f;
    ty = -height / 3.0f;
  }
  if (glfwGetKey('W') == GLFW_PRESS)
    ty -= height >> 4;
  if (glfwGetKey('A') == GLFW_PRESS)
    tx += width >> 4;
  if (glfwGetKey('S') == GLFW_PRESS)
    ty += height >> 4;
  if (glfwGetKey('D') == GLFW_PRESS)
    tx -= width >> 4;
  if (glfwGetKey('Q') == GLFW_PRESS)
    scale += 0.25f;
  if (glfwGetKey('E') == GLFW_PRESS)
    scale -= 0.25f;
  if (glfwGetKey(']') == GLFW_PRESS) {
    limit /= 2;
    if (limit < 1) limit = 2;
  }
  if (glfwGetKey('[') == GLFW_PRESS) {
    limit *= 2;
    if (limit > 1024) limit = 1024;
  }
}

int main(int argc, char* argv[])
{    
  Mandlebrot m(1024, 1024);
  m.render();
  return EXIT_SUCCESS;
}


/*
void TextureRenderer::handleMouse()
{
  glfwGetMousePos(&mx, &my);
  drag = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
  zoom = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}


void TextureRenderer::handleMouseMove()
{
  int x, y;
  glfwGetMousePos(&x, &y);
  if (drag) {
    tx += (mx - x);
    ty += (y - my);
    mx = x;
    my = y;
  } else if (zoom) {
    if (mx - x < 0)
      scale -= 0.1f;
    else if (mx - x > 0)
      scale += 0.1f;
    else
    {} // Nothing
  }
}*/



/*
TextureRenderer::TextureRenderer(int width, int height)
  : bbox(0, 0, width, height)
{
  this->width = width;
  this->height = height;
  data = new unsigned char[width * height * 3];
  
  scale = 2.f;
  tx = -width*2/3;
  ty = -height/3;
  limit = 32;

  mx = my = 0;
  drag = zoom = false;
  running = true;
  rendererType = 1;
  
  elapsed_timer = 0.0;
  modeText = "1: CPU (single thread)";
  computeText = "Time/fractal: ";
  fpsText = "Time/frame: ";

  glfwInit();
}*/

/*
void TextureRenderer::drawText(int x, int y, const string& text)
{
  // glRasterPos2i(x, y);
  // for (int i = 0; i < (int)text.size(); i++)
    //glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
  // TODO: If time allows, print to screen.
  cout << text << endl;
}*/