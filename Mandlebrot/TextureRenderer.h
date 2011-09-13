/// Author: Xavier Ho (contact@xavierho.com)
#pragma once
#ifdef _WIN32
  #include "../include/pthread.h"
#else
  #include <pthread>
#endif

/// Provides a simple interface to draw texture to the screen via a fullscreen
/// quad.  This renderer assumes 3 8-bit BGR channels in the texture, tightly packed.
/// This class prototype is rendering-library-neutral.
class TextureRenderer
{
  /// Texture
  unsigned char * data;
  int img_width, img_height;
  unsigned int texture_id;

  /// Rendering context
  bool running;
  bool has_texture;
  
public:
  TextureRenderer();
  TextureRenderer(int width, int height, unsigned char * data);
  virtual ~TextureRenderer();
  
  pthread_t start();
  void stop(pthread_t id);
  
  void setWindowSize(int width, int height);
  void setTexture(int width, int height, unsigned char * data);
  void setTexture(unsigned char *data);
 
private:
  /// Main loop functions
  static void * mainloop_threaded(void * obj);
  void mainloop();

protected:
  virtual void calculate();
  void render();
  virtual void handleInputs();
};