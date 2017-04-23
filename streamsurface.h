#ifndef STREAMSURFACE_H
#define STREAMSURFACE_H

#include "streamline.h"

using namespace std;
typedef vector<Point3D> Polyline3D;

class StreamSurface : public iterator<input_iterator_tag, int>
{
    vector<StreamLine> current_line;    // a collection of 3d points (forming a 3d polyline) that can stream
    int size;                           // collection size
    int steps;                          // how many steps happened (iteration conditions)
    const fftw_real dz = 1.f;           // how much every streamline step advances in the z-axis

    Polyline3D current_polyline();   // transform the 2d streamline points into a 3d polyline
public:
    StreamSurface(vector<Point>, vectorialField*, int, fftw_real, bool);


    StreamSurface& operator++();                           // rule to generate next point
    StreamSurface operator++(int) {StreamSurface tmp(*this); operator++(); return tmp;}

    // iteration conditions
    bool operator==(const StreamSurface&);
    bool operator!=(const StreamSurface&);
    bool operator==(int);
    bool operator!=(int);

    // output
    Polyline3D operator*();
    vector<Polyline3D> step();            // advances the polyline, returning the generated quads



};

#endif // STREAMSURFACE_H
