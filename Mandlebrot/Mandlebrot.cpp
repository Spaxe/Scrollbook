/**
  Xavier Ho, s2674674
  contact@xavierho.com
*/
#ifdef _WIN32
  #include "../GL/glew.h"
  #include "../GL/glfw.h"
#else
  #include <GL/glew.h>
  #include <GL/glfw.h>
#endif

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>

#include "../Prime/Timer.h"
#include "Mandlebrot.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////
/// Main program entry
Main::Main()  // TODO give command line options for window
 : fractalTimer(), renderer(512, 512)
{
}

Main::~Main()
{
}

void Main::start()
{
  renderer.init();
  // Main program loop
  while (renderer.running) {
    // Runs the Mandlebrot calculations
    fractalTimer.start();
    profile();
    elapsed_fractalTimer = fractalTimer.getMilliseconds();
    renderer.drawFullscreenQuad();
    renderer.handleInputs();
  }
}

void Main::profile()
{
  if (renderer.rendererType == 1)
    mandlebrot_single();
  else if (renderer.rendererType == 2)
    mandlebrot_threaded(4);
  else
    mandlebrot_cuda();
}

void Main::mandlebrot_single()
{
  renderer.worker();
}

void Main::mandlebrot_threaded(int numThreads)
{
  renderer.__mandlebrot_threaded(numThreads);
}

// TODO
void Main::mandlebrot_cuda()
{

}


///////////////////////////////////////////////////////////////////////////
/// Mandlebrot class
Mandlebrot::Mandlebrot(int width, int height)
  : TextureRenderer(width, height)
{
  numWorkers = 4;
  workers = new pthread_t[numWorkers];
}


Mandlebrot::~Mandlebrot()
{
  delete[] workers;
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
    return 0;
  return i / (float)(limit);
}


void Mandlebrot::worker()
{
  BBox b = bbox;
  for (int j = b.y1; j < b.y2; ++j) {
    for (int i = b.x1; i < b.x2; ++i) {  
      float x = (float)(i + offsetx) / width * scale;
      float y = (float)(j + offsety) / height * scale;
      unsigned char value = (unsigned char)(pixel_at(x, y) * 255);
      data[j*height*3+i*3] = value;
      data[j*height*3+1+i*3] = value >> 1;
      data[j*height*3+2+i*3] = value >> 2;
    }
  }
}


void Mandlebrot::__mandlebrot_threaded(int numThreads)
{
  for (int i = 0; i < numThreads; ++i) {
    int err = pthread_create(&workers[i], NULL, 
                                    &Mandlebrot::__run_one_worker, this);
    if (err)
      fprintf(stderr, "ERROR: Failed to create thread with exit code %d", err);
  }
  for (int i = 0; i < numThreads; ++i) {
    int err = pthread_join(workers[i], NULL);
    if (err)
      fprintf(stderr, "ERROR: Failed to join thread %d with code %d", i, err);
  }
}


void * Mandlebrot::__run_one_worker(void * obj)
{
  reinterpret_cast<Mandlebrot *>(obj)->worker();
  pthread_exit(NULL);
  return NULL;
}


TextureRenderer::TextureRenderer(int width, int height)
  : bbox(0, 0, width, height)
{
  this->width = width;
  this->height = height;
  data = new unsigned char[width * height * 3];
  
  scale = 2.f;
  offsetx = -width*2/3;
  offsety = -height/3;
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
}

TextureRenderer::~TextureRenderer()
{
  delete[] data;
  glfwTerminate();
}


void TextureRenderer::drawText(int x, int y, const string& text)
{
  // glRasterPos2i(x, y);
  // for (int i = 0; i < (int)text.size(); i++)
    //glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
  // TODO: If time allows, print to screen.
  cout << text << endl;
}


void TextureRenderer::init()
{
  /* Fire off GLFW */
  int err;
  err = glfwOpenWindow(width, height, 8, 8, 8, 8, 0, 0, GLFW_WINDOW);
  if (err != GL_TRUE)
    cerr << "Unable to initiate an OpenGL rendering context." << endl;
  glfwSetWindowTitle("Mandlebrot"); 
  glfwEnable(GLFW_AUTO_POLL_EVENTS);

  /* We will use a 2D texture and render the fractal as a fullscreen quad.
      The texture itself contains a copy of the buffer, on the GPU, and is
      rendered to screen */
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexImage2D(GL_TEXTURE_2D,
                0, 3, width, height, 0, GL_BGR_EXT, 
                GL_UNSIGNED_BYTE, data);

  /* OpenGL Settings */
  glMatrixMode(GL_PROJECTION);
  glOrtho(0, width, 0, height, -1.0, 1.0);
  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 0);
  glColor3f(1, 1, 1);
}


void TextureRenderer::drawFullscreenQuad()
{
  /* Updates fractal to the GPU memory and display */
  glTexSubImage2D(GL_TEXTURE_2D, 
                  0, 0, 0, width, height, GL_BGR_EXT, 
                  GL_UNSIGNED_BYTE, data);

  /* Draw the fullscreen quad */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_TEXTURE_2D);
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

  ///////////////////////////////////////////////////////////
  // Debug stuff
  // elapsed_timer = timer.getMilliseconds();
  // ostringstream fpsBuffer, msBuffer, fractalBuffer;
  // msBuffer << elapsed_timer;
  // fpsBuffer << 1 / elapsed_timer * 1000;
  // fractalBuffer << elapsed_fractalTimer;
  // fpsText = "Time/frame: " + msBuffer.str() + " ms  (" + fpsBuffer.str() + " fps)";
  // computeText = "Time/fractal: " + fractalBuffer.str() + " ms";
  // glDisable(GL_TEXTURE_2D);
  // drawText(0, height-13, modeText);
  // drawText(0, height-26, computeText);
  // drawText(0, height-39, fpsText);
  cout << "Rendered one frame." << endl;
  glfwSwapBuffers();
}


void TextureRenderer::handleInputs()
{
  handleKeys();
  handleMouse();
  handleMouseMove();
}


void TextureRenderer::handleKeys()
{
  if (glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS) // ESC
    running = false;
  if (glfwGetKey('H') == GLFW_PRESS) {
    scale = 2.f;
    offsetx = -width*2/3;
    offsety = -height/3;
  }
  if (glfwGetKey('W') == GLFW_PRESS)
    offsety -= height >> 4;
  if (glfwGetKey('A') == GLFW_PRESS)
    offsetx += width >> 4;
  if (glfwGetKey('S') == GLFW_PRESS)
    offsety += height >> 4;
  if (glfwGetKey('D') == GLFW_PRESS)
    offsetx -= width >> 4;
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
  if (glfwGetKey('1') == GLFW_PRESS)
    rendererType = 1;
  if (glfwGetKey('2') == GLFW_PRESS)
    rendererType = 1;
  if (glfwGetKey('3') == GLFW_PRESS)
    rendererType = 1;
}


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
    offsetx += (mx - x);
    offsety += (y - my);
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
}

    
int main(int argc, char* argv[])
{    
  Main().start();
  return EXIT_SUCCESS;
}


