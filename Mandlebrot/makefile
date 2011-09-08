LIBS= -lGL -lpthread -lGLU -lglut -lglew

all: Mandlebrot

Mandlebrot: Mandlebrot.cpp 
	gcc -Wall -g -c -o Mandlebrot.o Mandlebrot.cpp
	gcc -o Mandlebrot $(LIBS) Mandlebrot.o 

clean:
	rm Mandlebrot.o Mandlebrot 
