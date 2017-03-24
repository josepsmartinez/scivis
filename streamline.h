#ifndef STREAMLINE_H
#define STREAMLINE_H
#include <iterator>
#include <math.h>
#include "fields.h"

class StreamLine : public std::iterator<std::input_iterator_tag, int>
{
    // internal parameters
    float x,y;
    int dim;
    int steps;
    vectorialField* v;
    const fftw_real dt = 0.25;

    void load_ix(int);
    int get_ix();
public:
    StreamLine(int, vectorialField*, int);                                   // constructor given an starting point
    StreamLine(const StreamLine& mit);     // copy constructor

    StreamLine& operator++();                           // rule to generate next point
    StreamLine operator++(int) {StreamLine tmp(*this); operator++(); return tmp;} // still dont know why this pattern is necessary

    bool operator==(const StreamLine&);             // iteration conditions
    bool operator!=(const StreamLine&);
    bool operator==(int);
    bool operator!=(int);
    int operator*();                                   // output
};

#endif // STREAMLINE_H
