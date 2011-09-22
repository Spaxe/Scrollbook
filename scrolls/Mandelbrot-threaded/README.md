Compiling on Windows
====================
The source code is accompanied by a Visual Studio 2010 project file, and a handful of 32-bit .dlls for OpenGL.  The program should compile from the project solution.

Compiling on Unix
=================
A makefile is on the top directory of the source code.  Before compiling, ensure that you have the following prerequisites installed on your machine:

    sudo apt-get install build-essential libglew1.5 libglew1.5-dev glew-utils libglfw-dev libglfw2 gcc-4.4 g++-4.4 libxi-dev libxi6 libxmu-dev libxmu-headers libxmu6 freeglut3 freeglut3-dev

Then, just run

    make

Threading.h
===========
We have implemented a quick version of a Threading class in C++, because <pthread.h> was not designed with C++ in mind, and takes a bit of hacking for it to work.  However, later we discovered C++0x (and now C++11 as we write) supplies std::thread.  It looks much simpler to use, but we have not adapted our code to C++0x yet.

Rendering solution
==================
Instead of plotting each pixel into the device (which has a lot of transferring overhead), we instead draw a 'full-screen quad' with a texture applied to it.  A full-screen quad is a rectangle that matches the exact size of the viewport.  The texture is our rendered Mandelbrot set buffer, which is a single transfer and much, much faster than per-pixel transfer.

User controls
=============
The program supports a number of user interaction controls.

  -	Use W, A, S, D keys to pan around.
  -	Q and E zooms in and out.
  -	[ and ] changes the maximum iteration limit, up to 1024.
  -	H will bring the screen back to 'home view', the default viewport range.