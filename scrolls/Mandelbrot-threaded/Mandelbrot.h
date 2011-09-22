///
/// Author: Xavier Ho (contact@xavierho.com)
///
#pragma once
#include "TextureRenderer.h"
  
/**
 * rendering bounding box region
 */
struct BBox {
  int x1;
  int y1;
  int x2;
  int y2;
  
  BBox(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
};


/// For a point C(r, i) on the complex plane where
///
///   -2.5 < r < 1
///   -1   < i < 1
///
/// the Mandelbrot fractal is defined by the following algorithm:
///
///   count = 0
///   Z = 0     (Z is a complex number)
///   while |Z|<2 and count < some limit:
///     Z = Z^2 + C
///     count++
///   if |Z| < 2:
///     return 0
///   else:
///     return count      (intensity of the pixel)
///
class Mandelbrot : public TextureRenderer
{
  int limit;              /// Upper bound number of computing interations per pixel
  float scale;            /// Global scale of the renderer
  float tx;               /// Global translation on x axis
  float ty;               /// Global translation on y axis

public:
  Mandelbrot(int width, int height);
  virtual ~Mandelbrot();

private:
  /// Returns the pixel intensity at imaginary plane (cr, ci)
  /// The Mandelbrot fractal is embarrassingly parallel---one could compute it
  /// pixel by pixel with no interference.  The formula is simple and implemented below.
  /// This function returns the fractal at (cr, ci) in the range [0, 1]
  float pixel_at(float cr, float ci); 

  /// Threaded mandlebrot rendering function.
  void thread_action(int index);

  /// Grabs user inputs and provides feedback
  void handle_inputs();

  /// No multi-threading drawing method
  void draw();
};