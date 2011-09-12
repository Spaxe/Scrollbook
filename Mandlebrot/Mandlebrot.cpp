/**
  Xavier Ho, s2674674
  contact@xavierho.com
*/
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>

#ifdef _WIN32
  #include "../GL/glew.h"
  #include "../GL/glut.h"
#else
  #include "../GL/glew.h"
  #include <GL/glut.h>
#endif

#include "../Prime/Timer.h"
#include "Mandlebrot.h"
using namespace std;

///////////////////////////////////////////////////////////////////////////
/// Main program entry
Main::Main()
 : renderer(512, 512) // TODO give command line options for window
{
  rendererType = 1;
  numWorkers = 4;
  workers = new pthread_t[numWorkers];
}

Main::~Main()
{
  delete[] workers;
}

void Main::start()
{
  renderer.init();
}

void Main::profile()
{
  fractalTimer.start();
  if (rendererType == 1)
    mandlebrot_single();
  else if (rendererType == 2)
    mandlebrot_threaded(4);
  else
    mandlebrot_cuda();
  elapsed_fractalTimer = fractalTimer.getMilliseconds();
  renderer->drawFullscreenQuad();
}

void Main::mandlebrot_single()
{
  Mandlebrot::worker(this->renderer->bbox);
}

void Main::mandlebrot_threaded(int numThreads)
{
  for (int i = 0; i < numThreads; ++i) {
    int err = pthread_create(&workers[i], NULL, mandlebrotWorker, (void *)i);
    if (err)
      fprintf(stderr, "ERROR: Failed to create thread with exit code %d", err);
  }
  for (int i = 0; i < numThreads; ++i) {
    int err = pthread_join(workers[i], NULL);
    if (err)
      fprintf(stderr, "ERROR: Failed to join thread %d with code %d", i, err);
  }
}


// TODO
void Main::mandlebrot_cuda()
{

}


///////////////////////////////////////////////////////////////////////////
/// Mandlebrot class
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


void Mandlebrot::worker(BBox * bbox)
{
  for (int j = bbox->y1; j < bbox->y2; ++j) {
    for (int i = bbox->x1; i < bbox->x2; ++i) {  
      double x = (double)(i + offsetx) / width * scale;
      double y = (double)(j + offsety) / height * scale;
      unsigned char value = (unsigned char)(mandlebrot(x, y) * 255);
      data[j*height*3+i*3] = value;
      data[j*height*3+1+i*3] = value >> 1;
      data[j*height*3+2+i*3] = value >> 2;
    }
  }
}


void * Mandlebrot::run_one_worker(void * bbox)
{
  BBox * b = (BBox *) bbox;
  Mandlebrot::worker(b);
  pthread_exit(0);
}


TextureRenderer::TextureRenderer(int width, int height)
  : windowBBox(0, 0, width, height)
{
  this->width = width;
  this->height = height;
  data = new unsigned char[width * height * 3];
  
  scale = 2.f;
  offsetx = -width*2/3;
  offsety = -height/3;
  
  mx = my = 0;
  drag = zoom = false;
  renderer = 1;
  limit = 32;
  
  elapsed_timer = elapsed_fractalTimer = 0.0;
  modeText = "1: CPU (single thread)";
  computeText = "Time/fractal: ";
  fpsText = "Time/frame: ";
}

TextureRenderer::~TextureRenderer()
{
  delete[] data;
  glutDestroyWindow(glutGetWindow());
}


void TextureRenderer::drawText(int x, int y, const string& text)
{
  glRasterPos2i(x, y);
  for (int i = 0; i < (int)text.size(); i++)
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
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
  glutSwapBuffers();
  ///////////////////////////////////////////////////////////
  glutPostRedisplay();
}


void TextureRenderer::init(int argc, char * argv[])
{
  /* Fire off GLUT */
  
  glutInitWindowSize(width, height); 
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("Mandlebrot"); 

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
  
  /* GLUT Special callback functions */
  glutDisplayFunc(Main::profile);
  glutKeyboardFunc(TextureRenderer::handleKeys);
  glutMouseFunc(TextureRenderer::handleMouse);
  glutMotionFunc(TextureRenderer::handleMouseMove);

  glutMainLoop();
}


void TextureRenderer::handleKeys(unsigned char key, int x, int y)
{
  switch (key) {
  case 27: /* ESC */
    cleanup();
    exit(EXIT_SUCCESS);
    break;
  case 'h': /* Home view */
    scale = 2.f;
    offsetx = -width*2/3;
    offsety = -height/3;
    break;
  case 'w': /* ^<V> */
    offsety += height >> 4;
    break;
  case 'a':
    offsetx -= width >> 4;
    break;
  case 's':
    offsety -= height >> 4;
    break;
  case 'd':
    offsetx += width >> 4;
    break;
  case 'q': /* Zoom out and in */
    scale += 0.25f;
    break;
  case 'e':
    scale -= 0.25f;
    break;
  case '-': /* Quality settings */
    limit /= 2;
    if (limit < 1) limit = 2;
    break;
  case '+':
    limit *= 2;
    if (limit > 1024) limit = 1024;
    break;
  case '1': /* Serial */
    renderer = 1;
    break;
  case '2': /* Threaded */
    renderer = 2;
    break;
  case '3': /* CUDA */
    renderer = 3;
    break;
  default:
    break;
  }
}


void TextureRenderer::handleMouse(int button, int state, int x, int y)
{
  mx = x;
  my = y;
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    drag = true;
  else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
    zoom = true;
  else {
    zoom = false;
    drag = false;
  }
}


void TextureRenderer::handleMouseMove(int x, int y)
{
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
  glutInit(&argc, argv);
  return EXIT_SUCCESS;
}


