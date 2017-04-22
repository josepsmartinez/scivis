#ifndef CURVEPL_H
#define CURVEPL_H

#include "streamline.h"

using namespace std;
class Polyline
{
private:
    vector<Point> line;
public:
    Polyline();
};

// streamsurface: iterator that generates quads (can be a polyline) and stores streamlines (use step as "z" coordinate)
//  create streamlines from polyline points

class StreamSurface : public iterator<input_iterator_tag, int> {
    Polyline p;
public:
    StreamSurface(vector<Polyline>, vectorialField*, int);
    StreamSurface(const StreamSurface& mit);

    StreamSurface& operator++();
    StreamSurface operator++(int) {StreamSurface tmp(*this); operator++(); return tmp;}

};

#endif // CURVEPL_H
