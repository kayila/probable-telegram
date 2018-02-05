SOURCES=$(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS=main.o runner/StreamingSidecarRunner.o runner/SidecarRunner.o runner/FunctionParams.o
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

runner/%.o: src/main/runner/%.cpp
	mkdir -p runner
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	find -name '*.o' -delete
	rm -rf ./build

.PHONY: all dirs clean
