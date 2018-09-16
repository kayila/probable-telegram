#ifndef HEADERTOOBIGEXCEPTION_H
#define HEADERTOOBIGEXCEPTION_H

#include <exception>

class HeaderTooBigException : public std::exception {
public:
    const char* what();
};

#endif /* HEADERTOOBIGEXCEPTION_H */
