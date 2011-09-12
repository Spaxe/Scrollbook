OUTPUT_DIR=../lib/

all: Timer 

Timer: Timer.cpp
	gcc -Wall -fPIC -o Timer.o -c Timer.cpp
	gcc -shared -Wl,-soname,libTimer.so.1 -o libTimer.so.1.0.1 Timer.o

install:
	cd ../lib/; \
	ln -s ../Prime/libTimer.so.1.0.1 libTimer.so

remove:
	rm -f ../lib/libTimer.so.*	

clean:
	rm -f Timer.o libTimer.a
