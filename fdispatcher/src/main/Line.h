#ifndef LINE_H
#define LINE_H

class Line {
    public:
        Line(unsigned int x);
        ~Line();
        Line& start(unsigned int x);
        unsigned int start();
        Line& end(unsigned int x);
        unsigned int end();
        Line& found(bool x);
        bool found();
        unsigned int length();
        Line& line(char* x);
        Line& line(char* x, unsigned int size);
        char* line();
    private:
        unsigned int mStart;
        unsigned int mEnd;
        bool mFound;
        unsigned int mLength;
        char* mLine;
};

#endif /* LINE_H */

