/**
 * Timer.h
 * Simple timer implementation. 
 * Author: Xavier Ho (contact@xavierho.com)
 * Last updated: 15 Sep 2011
 */
#pragma once
#ifdef _WIN32 
  #include <windows.h>
#else          
  #include <sys/time.h>
#endif

/**
 * This class is able to handle resolutions down to micro-seconds.
 * To use it, simply call its start() and stop() methods, and then
 * query the resolution you prefer. It also computes FPS given the
 * number of frames rendered.
 *
 * Inspiration taken from Song Ho Ahn's notes.
 * http://www.songho.ca/misc/timer/timer.html
 */
class Timer
{
#ifdef _WIN32
  LARGE_INTEGER frequency, t1, t2;
#else
  timeval t1, t2;
#endif
  bool stopped;
  double ratio;

public:
  Timer() {
#ifdef _WIN32
    QueryPerformanceFrequency(&frequency);
#endif
    ratio = 1.0;
    stopped = true;
  }

  ~Timer() {}

  void start() {
#ifdef _WIN32
    QueryPerformanceCounter(&t1);
#else
    gettimeofday(&t1, NULL);
#endif
    stopped = false;
  }

  void stop() {
#ifdef _WIN32
    QueryPerformanceCounter(&t2);
#else
    gettimeofday(&t2, NULL);
#endif
    stopped = true;
  }

  double getSeconds();

  double getMilliseconds() {
    return getMicroseconds() * 0.001;
  }

  double getMicroseconds() {
    if (!stopped)
      stop();
#ifdef _WIN32
    double delta = (t2.QuadPart - t1.QuadPart) * 1000000.0 / frequency.QuadPart;
#else
    double delta = (t2.tv_sec - t1.tv_sec) * 1000000.0;
      delta += (t2.tv_usec - t1.tv_usec);
#endif
    return delta;
  }

  /**
   * @param framesRendered the number of frames rendered so far.
   * @return Frames per second.
   */
  double getFPS(int framesRendered){
    return framesRendered / getSeconds();
  }
  // @TODO: Time effects, slow down and speed up, etc.
};
