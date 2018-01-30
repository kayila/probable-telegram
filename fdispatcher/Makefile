SOURCES=$(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS=main.o
INCLUDES=-I.

CC=g++
CFLAGS=-static -Wall

all: build/fdispatch

dirs:
	mkdir -p build

build/fdispatch: dirs $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o build/fdispatch

%.o: src/main/%.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#all:
#	$(CC) $(CFLAGS) ${SOURCES} -o build/fdispatcher

clean:
	find -name '*.o' -delete
	rm -rf ./build

.PHONY: all dirs clean
