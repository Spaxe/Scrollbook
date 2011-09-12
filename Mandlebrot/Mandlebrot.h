/**
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
 */
#pragma once

#ifdef _WIN32
  #include "../include/pthread.h" // Because windows doesn't have pthread.  What.
#else
  #include <pthread.h>
#endif

  
/**
 * Mandlebrot bounding box helper struct
 */
struct BBox {
  int x1;
  int y1;
  int x2;
  int y2;
  
  BBox(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
};


/**
 * Texture Renderer simply renders a texture to a full-screen quad.
 */
class TextureRenderer
{
protected:
  int width;                        /// Rendering context variables
  int height;
  
  /// User interactions
  float scale;
  int offsetx;
  int offsety;
  
  /// Mouse location
  int mx;
  int my;

  /// Controls
  bool drag;
  bool zoom;

  /// OpenGL housekeeping
  GLuint textureID;
  GLuint vert, frag, prog;

  /// Debug
  Time timer;
  double elapsed_timer;
  std::string modeText;
  std::string computeText;
  std::string fpsText;
  
public:
  // Window bounding box
  BBox windowBBox;
  unsigned char * data;             /// Texture buffer on the CPU
  
  TextureRenderer(int width, int height);
  virtual ~TextureRenderer();
  
  /*
   * Initialises the OpenGL rendering context and allocates buffer
   */
  void init();
  
  /*
   * Renders a fullscreen quad.
   */
  void drawFullscreenQuad();
  
  /*
   * Handles user keyboard input
   */
  void handleKeys(unsigned char key, int x, int y);
  
  /*
   * Handles user mouse input
   */
  void handleMouse(int button, int state, int x, int y);
  
  /*
   * Handles user mouse dragging input
   */
  void handleMouseMove(int x, int y);
  
  // Helper function, could be refactored out
  /*
   * Draws a string using GLUT's built-in bitmaps
   */
  void drawText(int x, int y, const std::string& text);
};


/**
 * Functions for rendering a Mandlebrot fractal.
 */
class Mandlebrot : public TextureRenderer
{
public:
  /// limit of the calculation loop
  int limit;


  /** 
   * Returns the pixel intensity at imaginary plane (cr, ci)
   * The Mandlebrot fractal is embarrassingly parallel---one could compute it
   * pixel by pixel with no interference.  The formula is simple and implemented below.
   * This function returns the fractal at (cr, ci) in the range [0, 1]
   */
  inline float pixel_at(float cr, float ci); 
  
  
  /**
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
  inline void worker(BBox * bbox);
  
  
  /*
   * worker function wrapper for pthread.
   */
  void * run_one_worker(void * bbox);
};


/**
 * Main controller class
 */
class Main
{
  int numWorkers;
  TextureRenderer renderer;
  pthread_t * workers;

  int rendererType;
  Time fractalTimer;
  double elapsed_fractalTimer;
  
public:
  Main();
  virtual ~Main();
  
  /**
   * Runs the mandlebrot fractal in the main process.
   */
  void mandlebrot_single();
  
  /**
   * Runs the mandlebrot fractal with new threads.
   */
  void mandlebrot_threaded(int numThreads);
  
  /**
   * Runs the Mandlebrot fractal with CUDA.
   */ 
  void mandlebrot_cuda();
  
  /**
   * Starts the program.
   */
  void start();
  
  /**
   * Starts profiling the functions via the renderer.
   */
  void profile();
};