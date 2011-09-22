Compiling on Windows
====================
The source code is accompanied by a Visual Studio 2010 project file, and a handful of 32-bit .dlls for OpenGL.  The program should compile from the project solution.

Compiling on Unix
=================
A makefile is on the top directory of the source code.  Before compiling, ensure that you have the following prerequisites installed on your machine:

(This is tested on Ubuntu 11.04 32-bit, running NVIDIA 8600M GT; not exactly the fresh install so there might be some libraries missing.)

Make sure all of the CUDA prerequisites are installed, following the tutorial here: 

[http://hdfpga.blogspot.com/2011/05/install-cuda-40-on-ubuntu-1104.html]

    sudo apt-get install build-essential libglew1.5 libglew1.5-dev glew-utils libglfw-dev libglfw2 gcc-4.4 g++-4.4 libxi-dev libxi6 libxmu-dev libxmu-headers libxmu6 freeglut3 freeglut3-dev

Then, just run

    make

In both `Mandelbrot-thraeded` and `Mandelbrot-CUDA`.  If there is any compiling errors, it could be a library is missing.  If that’s the case, please email [contact@xavierho.com] and attach a session log.  We will do our best to debug the installation process.

Threading.h
===========

We have implemented a quick version of a Threading class in C++, because <pthread.h> was not designed with C++ in mind, and takes a bit of hacking for it to work.  However, later we discovered C++0x (and now C++11 as we write) supplies std::thread.  It looks much simpler to use, but we have not adapted our code to C++0x yet.

Rendering solution
==================
Instead of plotting each pixel into the device (which has a lot of transferring overhead), we instead draw a ‘full-screen quad’ with a texture applied to it.  A full-screen quad is a rectangle that matches the exact size of the viewport.  The texture is our rendered Mandelbrot set buffer, which is a single transfer and much, much faster than per-pixel transfer.

User controls
=============
The program supports a number of user interaction controls.
-	Use W, A, S, D keys to pan around.
-	Q and E zooms in and out.
-	[ and ] changes the maximum iteration limit, up to 1024.
-	H will bring the screen back to ‘home view’, the default viewport range.

Ubuntu Testing Environment Dump
===============================
This is our Ubuntu 11.04 5-year-old laptop with NVIDIA CUDA 4.0 toolkit.  We used it to test that the program runs on Linux for submission purposes.

    bin/linux/release/deviceQuery Starting...  
      
     CUDA Device Query (Runtime API) version (CUDART static linking)  
      
    Found 1 CUDA Capable device(s)  
      
    Device 0: "GeForce 8600M GT"  
      CUDA Driver Version / Runtime Version          4.0 / 4.0  
      CUDA Capability Major/Minor version number:    1.1  
      Total amount of global memory:                 256 MBytes (268107776 bytes)  
      ( 4) Multiprocessors x ( 8) CUDA Cores/MP:     32 CUDA Cores  
      GPU Clock Speed:                               0.95 GHz  
      Memory Clock rate:                             400.00 Mhz  
      Memory Bus Width:                              128-bit  
      Max Texture Dimension Size (x,y,z)             1D=(8192), 2D=(65536,32768), 3D=(2048,2048,2048)  
      Max Layered Texture Size (dim) x layers        1D=(8192) x 512, 2D=(8192,8192) x 512  
      Total amount of constant memory:               65536 bytes  
      Total amount of shared memory per block:       16384 bytes  
      Total number of registers available per block: 8192  
      Warp size:                                     32  
      Maximum number of threads per block:           512  
      Maximum sizes of each dimension of a block:    512 x 512 x 64  
      Maximum sizes of each dimension of a grid:     65535 x 65535 x 1  
      Maximum memory pitch:                          2147483647 bytes  
      Texture alignment:                             256 bytes  
      Concurrent copy and execution:                 Yes with 1 copy engine(s)  
      Run time limit on kernels:                     Yes  
      Integrated GPU sharing Host Memory:            No  
      Support host page-locked memory mapping:       Yes  
      Concurrent kernel execution:                   No  
      Alignment requirement for Surfaces:            Yes  
      Device has ECC support enabled:                No  
      Device is using TCC driver mode:               No  
      Device supports Unified Addressing (UVA):      No  
      Device PCI Bus ID / PCI location ID:           1 / 0  
      Compute Mode:  
         < Default (multiple host threads can use ::cudaSetDevice() with device simultaneously) >  
      
    deviceQuery, CUDA Driver = CUDART, CUDA Driver Version = 4.0, CUDA Runtime Version = 4.0, NumDevs = 1, Device = GeForce 8600M GT  
