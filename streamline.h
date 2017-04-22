#ifndef STREAMLINE_H
#define STREAMLINE_H
#include <iterator>
#include <math.h>
#include "fields.h"

using namespace std;

class Point {
private:

public:
    vector<fftw_real> p;
    Point(int dim);
    Point(vector<fftw_real> src);
    Point(const Point& src);
    initialize(vector<fftw_real> src);

    fftw_real norm();
    normalize();

    scalar_mul(fftw_real s);
    pointwise_mul(vector<fftw_real> src);
    pointwise_sum(vector<fftw_real> src);

};

class StreamLine : public std::iterator<std::input_iterator_tag, int>
{
    // internal parameters
    Point p;
    vectorialField* v;
    int dim;
    int steps;
    const fftw_real dt = 0.05;

    void load_ix(int);
    int get_ix();
public:
    StreamLine(int, vectorialField*, int);                                   // constructor given an starting point
    StreamLine(const StreamLine& mit);     // copy constructor

    StreamLine& operator++();                           // rule to generate next point
    StreamLine operator++(int) {StreamLine tmp(*this); operator++(); return tmp;} // still dont know why this pattern is necessary

    // iteration conditions
    bool operator==(const StreamLine&);
    bool operator!=(const StreamLine&);
    bool operator==(int);
    bool operator!=(int);

    // output
    Point operator*();
    vector<Point> line(int);
};

#endif // STREAMLINE_H
