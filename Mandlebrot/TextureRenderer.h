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
#include "../Prime/Timer.h"
#include "Threading.h"

/// Simple renderer that draws a fullscreen quad with a texture.
/// Multithreading is supported.  This class is abstract, and the subclass must
/// override the following functions:
///
///   void thread_action()
///   void handle_inputs() (optional)
///   ThreadingHelper setup_arguments(int thread_index)
///   void cleanup_arguments(ThreadingHelper * helper)
///
/// See Threading.h for more information.
class TextureRenderer : public Threading
{
  unsigned int texture_id;  /// Internal texture id tracker
  Time timer;               /// Performance tracker
  
protected:
  double elapsed_time;      /// Total time took to render one frame, in ms

  /// True upon running.  Set to false when you want the program to finish.
  bool running;      
  unsigned char * data;     /// Texture data
  int width, height;        /// Texture resolution

  /// Number of threads currently running.  Do NOT directly modify this value.
  /// You can use it to compare against the thread_index in setup_arguements().
  int thread_count;
  
public:
  TextureRenderer(int width, int height);
  virtual ~TextureRenderer();

  void set_window_title(const char * text);
  void set_window_size(int width, int height);

  /// Starts one plus one thread;
  void start();

  /// Starts a multi-threaded program with additional count threads
  void start_threaded(int count);

private:
  void __start();
  void __set_texture();
  void render();

protected:
  /// Override this method to handle user inputs.
  virtual void handle_inputs();
};