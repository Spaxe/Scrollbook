Mandlebrot
==========
Simple Mandlebrot fractal renderer using OpenGL, pthread, and CUDA for performance comparisons.

Usage
-----
Left-click and drag to pan.

Right-click and drag to zoom in or zoom out.

Use + or - to increase/decrease the quality.

H key to return to default home view.


Building the program
--------------------
### Prerequisites
  - C++
  - OpenGL 1.2+ 
  - GLEW 1.7.0
  - FreeGLUT 2.6.0
  - CUDA (Soon)

### Windows
Use the VS2010 project files or link against the above libraries.

### Linux
-lGL -lGLU -lglew -lglut -lpthread

### Tested Environments
- Windows 7 64-bit
- Ubuntu 11 64-bit


Notes
-----
Windows: Run mklink_dll.bat prior to automatically generate links to the .dll files for you.