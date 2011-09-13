///
/// Author: Xavier Ho (contact@xavierho.com)
///
/// Provides a simple interface to draw texture to the screen via a fullscreen
/// quad.  This renderer assumes 3 8-bit BGR channels in the texture, tightly packed.
#pragma once
#ifdef _WIN32
  #include "../GL/glew.h"
  #include "../GL/glfw.h"
#else
  #include <GL/glew.h>
  #include <GL/glfw.h>
#endif
#include "Threading.h"

class TextureRenderer : public Threading
{
  unsigned int texture_id;

protected:
  bool running;
  unsigned char * data;
  int width, height;
  
public:
  TextureRenderer(int width, int height);
  virtual ~TextureRenderer();

  void set_window_title(const char * text);
  void set_window_size(int width, int height);
  void start();
  void start_threaded(int count);

private:
  void __start();
  void __set_texture();

protected:
  virtual void thread_action() = 0;
  void render();
  virtual void handle_inputs();
};