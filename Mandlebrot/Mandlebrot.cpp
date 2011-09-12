/**
  Xavier Ho, s2674674
  contact@xavierho.com
* /
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>

#ifdef _WIN32
  #include "../include/pthread.h" // Because windows doesn't have pthread.  What.
  #include "../GL/glew.h"
  #include "../GL/glut.h"
#else
  #include <pthread.h>
  #include "../GL/glew.h"
  #include <GL/glut.h>
#endif

#include "../Prime/Timer.h"
using namespace std;

/* Width and height of the window */
int width = 1024;   
int height = 1024;

/* Interaction states */
float scale = 2.f;
int offsetx = -width*2/3;
int offsety = -height/3;

/* Mouse location */
int mx = 0;
int my = 0;

/* Controls */
bool drag = false;
bool zoom = false;
int renderer = 1;

/* Internal buffer for the fractal */
unsigned char * data = NULL;
int limit = 32;

/* OpenGL housekeeping */
GLuint textureID = 0;
GLuint vert = 0, frag = 0, prog = 0;
bool cudaRendering = false;

/* Threading */
pthread_t * workers;
struct BBox {
  int x1;
  int y1;
  int x2;
  int y2;
};
BBox windowBox;


/* Debug */
Time timer;
Time fractalTimer;
double elapsed_timer = 0.0;
double elapsed_fractalTimer = 0.0;
string modeText = "1: CPU (single thread)";
string computeText = "Time/fractal: ";
string fpsText = "Time/frame: ";


/*
 * The Mandlebrot fractal is embarrassingly parallel---one could compute it
 * pixel by pixel with no interference.  The formula is simple and implemented below.
 * This function returns the fractal at (cr, ci) in the range [0, 1]
 */
double mandlebrot(double cr, double ci)
{
  double x = 0, y = 0;
  double tmp;
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


/*
 * CPU implementation of the mandlebrot set.
 * bbox is the bounding box of the retangle to be calculated by this
 * thread.  It should be a struct of 4 integers, such as:
 *   struct bbox {
 *     int x1,
 *     int x2,
 *     int y1,
 *     int y2
 *   }
 * The worker will render in the range from (x1, y1) to (x2-1, y2-1).
 * Note that the upper bound is exclusive.
 */
void * mandlebrotWorker(void * bbox)
{
  BBox * b = (BBox *) bbox;
  for (int j = b->y1; j < b->y2; ++j) {
    for (int i = b->x1; i < b->x2; ++i) {  
      double x = (double)(i + offsetx) / width * scale;
      double y = (double)(j + offsety) / height * scale;
      unsigned char value = (unsigned char)(mandlebrot(x, y) * 255);
      data[j*height*3+i*3] = value;
      data[j*height*3+1+i*3] = value >> 1;
      data[j*height*3+2+i*3] = value >> 2;
    }
  }
  if (renderer == 2)
    pthread_exit(0);
}

/*
 * Thread-spawning function.  Blocks until all threads have finished.
 */
void threaded(int numThreads)
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
  delete[] workers;
}


/*
 * CUDA-enabled implementation of the mandlebrot set.
 */
void cuda()
{

}


/*
 * Draws a string using GLUT's built-in bitmaps
 */
void drawText(int x, int y, const string& text)
{
  glRasterPos2i(x, y);
  for (int i = 0; i < (int)text.size(); i++)
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, text[i]);
}

/*
 * Rendering the Mandlebrot fractal on a fullscreen quad.
 */
void render()
{
  // profiling
  timer.start();

  if (data != NULL) {
    /* Computes the Mandlebrot fractal for this frame */
    fractalTimer.start();
    if (renderer == 1)
      mandlebrotWorker((void *)&windowBox);
    else if (renderer == 2)
      threaded(4);
    else
      cuda();
    elapsed_fractalTimer = fractalTimer.getMilliseconds();

    /* Updates fractal to the GPU memory and display */
    glTexSubImage2D(GL_TEXTURE_2D, 
                    0, 0, 0, width, height, GL_BGR_EXT, 
                    GL_UNSIGNED_BYTE, data);
  }
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
  elapsed_timer = timer.getMilliseconds();
  ostringstream fpsBuffer, msBuffer, fractalBuffer;
  msBuffer << elapsed_timer;
  fpsBuffer << 1 / elapsed_timer * 1000;
  fractalBuffer << elapsed_fractalTimer;
  fpsText = "Time/frame: " + msBuffer.str() + " ms  (" + fpsBuffer.str() + " fps)";
  computeText = "Time/fractal: " + fractalBuffer.str() + " ms";
  glDisable(GL_TEXTURE_2D);
  drawText(0, height-13, modeText);
  drawText(0, height-26, computeText);
  drawText(0, height-39, fpsText);
  glutSwapBuffers();
  ///////////////////////////////////////////////////////////
  glutPostRedisplay();
}


/*
 * Refresh up to 60FPS
 */
void idle()
{
  glutPostRedisplay();
}


/*
 * Initialises the OpenGL rendering context and allocates buffer
 */
void init()
{
  /* Allocate a BGR buffer at the window size */
  data = new unsigned char[width * height * 3];
  /* Initialise buffers for the threads */
  workers = new pthread_t[16];

  windowBox.x1 = 0;
  windowBox.y1 = 0;
  windowBox.x2 = width;
  windowBox.y2 = height;

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


/*
 * Free any resources claimed
 */
void cleanup()
{
  delete[] data;
  delete[] workers;
  glutDestroyWindow(glutGetWindow());
}


/*
 * Handles user keyboard input
 */
void handleKeys(unsigned char key, int x, int y)
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


/*
 * Handles user mouse input
 */
void handleMouse(int button, int state, int x, int y)
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


/*
 * Handles user mouse dragging input
 */
void handleMouseMove(int x, int y)
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
  /* Fire off GLUT */
  glutInit(&argc, argv);
  glutInitWindowSize(width, height); 
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("Mandlebrot"); 

  /* setup */
  init();

  /* GLUT Special callback functions */
  glutDisplayFunc(render);
  glutKeyboardFunc(handleKeys);
  glutMouseFunc(handleMouse);
  glutMotionFunc(handleMouseMove);
  glutIdleFunc(idle);

  glutMainLoop();
  return EXIT_SUCCESS;
}


