/**
  Xavier Ho, s2674674
  contact@xavierho.com
*/
#include <iostream>
#include <cmath>
#include <string>
#include "Mandelbrot.h"
using namespace std;

Mandelbrot::Mandelbrot(int width, int height)
  : TextureRenderer(width, height) 
{
  this->limit = 64;
  this->scale = 3.0f;
  this->tx = -width * 5 / 7.0f;
  this->ty = -height / 2.0f;
}

Mandelbrot::~Mandelbrot()
{
}

float Mandelbrot::pixel_at(float cr, float ci)
{
  float x = 0, y = 0;
  float tmp;
  int i = 0;
  while ((x*x + y*y < 4) && (i++ < limit)) {
      tmp = x*x - y*y + cr;
      y = 2 * x * y + ci;
      x = tmp;
  }
  if (i >= limit)
    return 0.0f;
  return i / (float)(limit);
}

void Mandelbrot::thread_action(int index)
{
  BBox bbox(0, 0, width, height);
  int block_height = height / thread_count;
  if (index + 1 < thread_count) {
    bbox.y1 = (block_height * index);
    bbox.y2 = (block_height * (index+1));
  } else {
    bbox.y1 = (block_height * index);
    bbox.y2 = height;
  }
  while (running) {
    for (int j = bbox.y1; j < bbox.y2; ++j) {
      for (int i = bbox.x1; i < bbox.x2; ++i) {  
        float x = (float)(i + tx) / width * scale;
        float y = (float)(j + ty) / height * scale;
        unsigned char value = (unsigned char)(pixel_at(x, y) * 255);
#ifdef DEBUG
        if (j == bbox.y1) {
          data[j*height*3+i*3] = 255;
          data[j*height*3+1+i*3] = 0;
          data[j*height*3+2+i*3] = 0;
        } else {
#endif
          data[j*height*3+i*3] = value;
          data[j*height*3+1+i*3] = value >> 1;
          data[j*height*3+2+i*3] = value >> 2;
#ifdef DEBUG
        }
#endif
      }
    }
    thread_signal_and_wait();
  }
}

void Mandelbrot::handle_inputs()
{
  TextureRenderer::handle_inputs();
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
  if (glfwGetKey('-') == GLFW_PRESS) {
    limit /= 2;
    if (limit < 1) limit = 2;
  }
  if (glfwGetKey('+') == GLFW_PRESS) {
    limit *= 2;
    if (limit > 1024) limit = 1024;
  }
}

int main(int argc, char* argv[])
{    
  Mandelbrot m(1024, 1024);
  m.start_threaded(128);
  return EXIT_SUCCESS;
}