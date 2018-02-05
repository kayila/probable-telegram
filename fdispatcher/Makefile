SOURCES=$(wildcard *.cpp) $(wildcard */*.cpp)
OBJECTS=main.o runner/StreamingSidecarRunner.o runner/FunctionParams.o
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

runner/StreamingSidecarRunner.o: src/main/runner/StreamingSidecarRunner.cpp
	mkdir -p runner
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

runner/FunctionParams.o: src/main/runner/FunctionParams.cpp
	mkdir -p runner
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

#all:
#	$(CC) $(CFLAGS) ${SOURCES} -o build/fdispatcher

clean:
	find -name '*.o' -delete
	rm -rf ./build

.PHONY: all dirs clean
