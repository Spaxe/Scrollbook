/**
 * Timer.h
 * Simple timer implementation. 
 * Author: Xavier Ho (contact@xavierho.com)
 * TODO: Refactor and redesign
 * Last updated: 08 Sep 2011
 */
#pragma once
#ifdef _WIN32 
    #include <windows.h>
#else          
    #include <sys/time.h>
#endif

#ifndef PRIME_EXPORTS
  #define DECLSPEC __declspec(dllimport)
#else
  #define DECLSPEC __declspec(dllexport)
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
class DECLSPEC Time
{
#ifdef _WIN32
    LARGE_INTEGER frequency, t1, t2;
#else
    timeval t1, t2;
#endif
    bool stopped;
    double ratio;

public:
    /**
     * Constructor.
     */
    Time();

    /**
     * Destructor.
     */
    ~Time();

    /**
     * Starts the timer. Without this function everything else is meaningless.
     */
    void start();

    /**
     * Stops the timer.
     */
    void stop();

    /**
     * @return the number of seconds passed.
     */
    double getSeconds();

    /**
     * @return the number of milliseconds passed.
     */
    double getMilliseconds();

    /**
     * @return the number of microseconds passed.
     */
    double getMicroseconds();

    /**
     * @param framesRendered the number of frames rendered so far.
     * @return Frames per second.
     */
    double getFPS(int framesRendered);

    // @TODO: Time effects, slow down and speed up, etc.
};