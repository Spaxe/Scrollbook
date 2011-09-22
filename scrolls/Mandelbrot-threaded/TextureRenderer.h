///
/// Author: Xavier Ho (contact@xavierho.com)
///
/// Provides a simple interface to draw texture to the screen via a fullscreen
/// quad.  This renderer assumes 3 8-bit BGR channels in the texture, tightly packed.
#pragma once
#ifdef _WIN32
  #include "GL/glew.h"
  #include "GL/glfw.h"
#else
  #include <GL/glew.h>
  #include <GL/glfw.h>
#endif
#include "Timer.h"
#include "Threading.h"

/// Simple renderer that draws a fullscreen quad with a texture.
///
/// Multithreading is supported.  Override this:
///
///   void thread_action(int index)
///
/// In addition, if you want to have more than ESC to quit, override:
///
///   void handle_inputs() (optional)
///
/// See Threading.h for more information.
class TextureRenderer : public Threading
{
  unsigned int texture_id;  /// Internal texture id tracker
  Timer timer;               /// Performance tracker

  /// Thread synchronisation stuff. Messy, could use some refactoring
  pthread_mutex_t count_mutex; 
  pthread_cond_t count_threshold_cv;
  volatile int resources;            /// Number of threads completed; for synchronisation
  
protected:
  double elapsed_time;      /// Total time took to render one frame, in ms

  /// True upon running.  Set to false when you want the program to finish.
  bool running;      
  unsigned char * data;     /// Texture data
  int width, height;        /// Texture resolution

public:
  TextureRenderer(int width, int height);
  virtual ~TextureRenderer();

  void set_window_title(const char * text);
  void set_window_size(int width, int height);

  /// Starts a multi-threaded program with additional count threads
  void start_threaded(int count);

private:
  void __start();
  void __set_texture();
  void render();

protected:
  /// Override this method to handle user inputs.
  virtual void handle_inputs();

  /// Thread synchronisation.  Call in thread_action() to signal finish.
  void thread_signal_and_wait();
};
