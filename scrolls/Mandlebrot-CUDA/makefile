LIBS= -lGL -lpthread -lGLU -lglut -lTimer # -lglew
LIB_PATH=-L../lib/

all: Mandlebrot

Mandlebrot: Mandlebrot.cpp 
	gcc -Wall -g -c -o Mandlebrot.o Mandlebrot.cpp
	gcc -o Mandlebrot $(LIBS) $(LIB_PATH) Mandlebrot.o 

install:
	ln -s ../lib/libTimer.so libTimer.so.1

remove:
	rm -f libTimer.so.*

clean:
	rm -f Mandlebrot.o Mandlebrot  
