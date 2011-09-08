/**
  4305ENG Advanced Computer Systems
  Assignment 1 - Drawing the Mandlebrot fractal
  Xavier Ho, s2674674
  contact@xavierho.com

  For a point C(r, i) on the complex plane where
     -2.5 < r < 1
     -1   < i < 1
  the Mandlebrot fractal is defined by the following algorithm:
     count = 0
     Z = 0     (Z is a complex number)
     while |Z|<2 and count < some limit:
       Z = Z^2 + C
       count++
     if |Z| < 2:
       return 0
     else:
       return count      (intensity of the pixel)

  Dependencies:
    OpenGL 2.0+
    FreeGLUT 2.6+
    GLEW
    CUDA (Optional)
 */
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstring>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../Prime/Timer.h"


/* Width and height of the window */
int width;   
int height;
/* Interaction states */
float scale;
int offsetx;
int offsety;
/* Mouse location */
int mx;
int my;
/* Controls */
bool drag;
bool zoom;
int renderer;
/* Internal buffer for the fractal */
unsigned char * data;
int limit;
/* OpenGL housekeeping */
GLuint textureID;
GLuint vert, frag, prog;
int cudaRendering;
Time timer;
int timer_count;
double elapsed_timer;


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
 * 1. Non-parallel implementation of the mandlebrot set
 */
void serial()
{
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {  
      double x = (double)(i + offsetx) / width * scale;
      double y = (double)(j + offsety) / height * scale;
      unsigned char value = (unsigned char)(mandlebrot(x, y) * 255);
      data[j*height*3+i*3] = value;
      data[j*height*3+1+i*3] = value >> 1;
      data[j*height*3+2+i*3] = value >> 2;
    }
  }
}


/*
 * 2. pthread implementation of the mandlebrot set.
 */
void threaded()
{

}


/*
 * CUDA-enabled implementation of the mandlebrot set.
 */
void cuda()
{

}


/*
 * Rendering the Mandlebrot fractal on a fullscreen quad.
 */
void render()
{
  // profiling
  if (timer_count == 0)
    timer.start();

  if (data != NULL) {
    /* Computes the Mandlebrot fractal for this frame */
    if (renderer == 1)
      serial();
    else if (renderer == 2)
      threaded();
    else
      cuda();
    /* Updates fractal to the GPU memory and display */
    glTexSubImage2D(GL_TEXTURE_2D, 
                    0, 0, 0, width, height, GL_BGR_EXT, 
                    GL_UNSIGNED_BYTE, data);
  }
  /* Draw the fullscreen quad */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_COLOR_BUFFER_BIT);
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
  glutPostRedisplay();

  if (timer_count++ == 30) {
    printf("Time: %.2f ms\n", elapsed_timer / 30); 
    timer_count = 0;
    elapsed_timer = 0.0;
  } else {
    elapsed_timer += timer.getMilliseconds();
    timer.start();
    glutSwapBuffers();
  }
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
  glEnable(GL_TEXTURE_2D);
  glOrtho(0, width, 0, height, -1.0, 1.0);
  glViewport(0, 0, width, height);
  glClearColor(0, 0, 0, 0);
}


/*
 * Free any resources claimed
 */
void cleanup()
{
  delete[] data;
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
  width = 1024;
  height = 1024;
  scale = 2.f;
  offsetx = -width*2/3;
  offsety = -height/3;
  limit = 32;
  cudaRendering = false;
  drag = false;
  zoom = false;
  timer_count = 0;
  renderer = 1;
  elapsed_timer = 0.0;
    
  /* Fire off GLUT */
  glutInit(&argc, argv);
  glutInitWindowSize(width, height); 
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("Mandlebrot"); 

  /* OpenGL setup */
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


