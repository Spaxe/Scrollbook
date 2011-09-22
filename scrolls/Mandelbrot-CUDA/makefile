LIBS= -lGL -lpthread -lGLU -lGLEW -lglfw
LIB_PATH=-L./lib/
INC_PATH=-I./include/

all: Mandelbrot

Mandelbrot: Mandelbrot.cu 
	nvcc $(INC_PATH) $(LIB_PATH) $(LIBS) -o Mandelbrot Mandelbrot.cu

clean:
	rm -f Mandelbrot  
