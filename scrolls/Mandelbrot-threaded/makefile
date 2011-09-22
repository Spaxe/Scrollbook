LIBS= -lGL -lpthread -lGLU -lGLEW -lglfw
LIB_PATH=-L./lib/
INC_PATH=-I./include/

all: Mandelbrot

Mandelbrot: Mandelbrot.cpp TextureRenderer.cpp
	gcc -Wall -c $(INC_PATH) -o TextureRenderer.o TextureRenderer.cpp
	gcc -Wall -c $(INC_PATH) -o Mandelbrot.o Mandelbrot.cpp
	gcc -o Mandelbrot $(LIBS) $(LIB_PATH) Mandelbrot.o TextureRenderer.o

clean:
	rm -f Mandelbrot.o TextureRenderer.o Mandelbrot  
