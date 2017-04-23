#include "streamsurface.h"

Polyline3D StreamSurface::current_polyline()
{
    Polyline3D newline;
    newline.resize(size);

    for(int i=0; i<size; i++){
        //Point streampoint(*(current_line[i]));
        newline[i] = current_line[i].point3D(dz);
    }
}

StreamSurface::StreamSurface(vector<Point> ini_line, vectorialField* srcv, int dim, fftw_real dt=0.3f, bool norm=true)
    : size(ini_line.size())
{
    for (int i=0; i < size; i++)
        current_line.push_back(StreamLine(ini_line[i].p[0], ini_line[i].p[1], srcv, dim, dt, norm));
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

vector<Polyline3D> StreamSurface::step()
{
    Polyline3D old_line = current_polyline();
    (*this)++;
    Polyline3D cur_line = current_polyline();

    vector<Polyline3D> quads;
    for(int i=0; i < size-1; i++) {
        quads.push_back({old_line[i], old_line[i+1], cur_line[i], cur_line[i+1]});
    }
    return quads;
}
