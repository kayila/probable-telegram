#include "headertoobigexception.h"

const char* HeaderTooBigException::what() {
    return "Header size is too large.";
}
