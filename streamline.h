#ifndef STREAMLINE_H
#define STREAMLINE_H
#include <iterator>
#include <math.h>
#include "fields.h"

using namespace std;

class Point {
private:

protected:

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

class Point3D : public Point {
public:

    Point3D();
    Point3D(vector<fftw_real> src);
    Point3D(const Point& src);
    Point3D(const Point3D& src);
};

class StreamLine : public std::iterator<std::input_iterator_tag, int>
{
    // internal parameters
    vectorialField* v;
    int dim;
    int steps;
    fftw_real dt;
    bool end = false;
    bool normalize = true;

    void load_ix(int);
    int get_ix();
public:
    Point current;
    Point start;


    StreamLine(float, float, vectorialField*, int, fftw_real, bool);    // constructor given an starting point, an field, its dimension, dt and wether the shift must be normalized
    StreamLine(const StreamLine& mit);     // copy constructor

    StreamLine& operator++();                           // rule to generate next point
    StreamLine operator++(int) {StreamLine tmp(*this); operator++(); return tmp;} // still dont know why this pattern is necessary

    void change_field(vectorialField* nv);

    // iteration conditions
    bool operator==(const StreamLine&);
    bool operator!=(const StreamLine&);
    bool operator==(int);
    bool operator!=(int);

    // output
    Point operator*();
    vector<Point> line(int, int lenght);
    Point3D point3D(fftw_real);
};

#endif // STREAMLINE_H
