///
/// Author: Xavier Ho (contact@xavierho.com)
///
#include <iostream>
#include <cmath>
#include <string>
#include "GL\glew.h"
#include "GL\glfw.h"
#include "Mandlebrot.h"
using namespace std;

Mandlebrot::Mandlebrot(int width, int height)
{
  this->width = width;
  this->height = height;
  this->limit = 64;
  this->scale = 3.0f;
  this->tx = -width * 5 / 7.0f;
  this->ty = -height / 2.0f;
  this->data = new unsigned char[width * height * 3];
}

Mandlebrot::~Mandlebrot()
{
  delete[] this->data;
}

float Mandlebrot::pixel_at(float cr, float ci)
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

void Mandlebrot::render()
{
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {  
      float x = (float)(i + tx) / width * scale;
      float y = (float)(j + ty) / height * scale;
      unsigned char value = (unsigned char)(pixel_at(x, y) * 255);
      data[j*height*3+i*3] = value;
      data[j*height*3+1+i*3] = value >> 1;
      data[j*height*3+2+i*3] = value >> 2;
    }
  }
}

void Mandlebrot::handle_inputs()
{
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
  Mandlebrot m(1024, 1024);
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