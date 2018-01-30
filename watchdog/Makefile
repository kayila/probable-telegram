SOURCES=$(wildcard src/main/*.cpp)

all:
	mkdir -p build
	g++ -static ${SOURCES} -o build/probable-telegram

clean:
	find -name '*.o' -delete
	rm -f ./probable-telegram
