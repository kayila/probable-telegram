SOURCES=$(wildcard src/main/*.cpp)

all:
	mkdir -p build
	g++ -static ${SOURCES} -o build/fdispatcher

clean:
	find -name '*.o' -delete
	rm -rf ./build
