///
/// Author: Xavier Ho (contact@xavierho.com)
///
#pragma once

struct Prop {

};

/// For a point C(r, i) on the complex plane where
///
///   -2.5 < r < 1
///   -1   < i < 1
///
/// the Mandlebrot fractal is defined by the following algorithm:
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
class Mandlebrot
{
  int limit;              /// Upper bound number of computing interations per pixel
  float scale;            /// Global scale of the renderer
  float tx;               /// Global translation on x axis
  float ty;               /// Global translation on y axis
  bool running;           /// Rendering state
  int width;              /// Window size
  int height;
  size_t size;            /// Size of data buffer
  unsigned char * data;     /// System memory for the fractal
  unsigned char * gpu_data; /// Device memory for the fractal
  unsigned int texture_id;  /// OpenGL Texture ID
  Timer timer;
  float elapsed_time;

public:
  Mandlebrot(int width, int height);
  virtual ~Mandlebrot();

  void render();

private:
  /// Returns the pixel intensity at imaginary plane (cr, ci)
  /// The Mandlebrot fractal is embarrassingly parallel---one could compute it
  /// pixel by pixel with no interference.  The formula is simple and implemented below.
  /// This function returns the fractal at (cr, ci) in the range [0, 1]
  float pixel_at(float cr, float ci); 

  /// Grabs user inputs and provides feedback
  void handle_inputs();
};