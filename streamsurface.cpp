#include "streamsurface.h"
#include <QtWidgets>

Polyline3D StreamSurface::current_polyline()
{
    Polyline3D newline;
    newline.resize(size);

    for(int i=0; i<size; i++){
        //Point streampoint(*(current_line[i]));
        newline[i] = current_line[i].point3D(dz);

    }
    return newline;
}

StreamSurface::StreamSurface(vector<Point> ini_line, vectorialField* srcv, int dim, fftw_real dt=0.3f, bool norm=true)
    : size(ini_line.size())
{
    for (int i=0; i < size; i++){
        StreamLine cur(ini_line[i].p[0], ini_line[i].p[1], srcv, dim, dt, norm);
        current_line.push_back(cur);
    }
}

StreamSurface &StreamSurface::operator++()
{
    for(int i=0; i<size; i++){
        current_line[i]++;
    }

}

bool StreamSurface::operator==(const StreamSurface &other)
{
    return( steps == other.steps );
}

bool StreamSurface::operator!=(const StreamSurface &other)
{
    return( steps != other.steps );
}

bool StreamSurface::operator==(int s)
{
    return( steps == s );
}

bool StreamSurface::operator!=(int s)
{
    return( steps != s );
}

Polyline3D StreamSurface::operator*()
{
    return current_polyline();
}

Polyline3D StreamSurface::step()
{
    (*this)++;
    return current_polyline();
}
