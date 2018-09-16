#include "Line.h"
#include <stdexcept>
#include <string.h>

Line::Line(unsigned int x) {
    mStart = 0;
    mEnd = 0;
    mFound = false;
    mLength = x;
    mLine = (char*) malloc(mLength * sizeof(char*));
}

Line::~Line() {
    free(mLine);
}

Line* Line::start(unsigned int x) {
    mStart = x;
    return this;
}

unsigned int Line::start() {
    return mStart;
}

Line* Line::end(unsigned int x) {
    mEnd = x;
    return this;
}

unsigned int Line::end() {
    return mEnd;
}

Line* Line::found(bool x) {
    mFound = x;
    return this;
}

bool Line::found() {
    return mFound;
}

unsigned int Line::length() {
    return mLength;
}

Line* Line::line(char* x) {
    return line(x, mLength);
}

Line* Line::line(char* x, unsigned int size) {
    if (size > mLength) {
        throw std::length_error("Size is too large for this line");
    }
    strncpy(mLine, x, size);
    return this;
}

char* Line::line() {
    return mLine;
}
