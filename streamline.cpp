#include "streamline.h"

// (x,y) <-> ix
void StreamLine::load_ix(int ix)
{
    x = (float) (ix % dim);
    y = (float) (ix / dim);
}

int StreamLine::get_ix()
{
    return ((int)x) + ((int)y)*dim;
}


// CONSTRUCTORS
StreamLine::StreamLine(int ix, vectorialField* v, int dim)
    : v(v), dim(dim), steps(0)
{
    /*
     * here we can decide whether the stream follows
     *  a flowing velocity field (how it is implemented, every iteration acessing the updated v field)
     *  a static, predictive velocity field (implies allocating and copying the field)
    */
    load_ix(ix);
}

StreamLine::StreamLine(const StreamLine &mit)
    : v(mit.v), dim(mit.dim), steps(mit.steps), x(mit.x), y(mit.y), dt(mit.dt)
{
}

// OPERATORS
StreamLine &StreamLine::operator++()
{
    x += std::max(std::min(v->read_x(x)*dt, (fftw_real)dim-1), (fftw_real)0.f);
    y += std::max(std::min(v->read_y(y)*dt, (fftw_real)dim-1), (fftw_real)0.f);

    steps++;
}


bool StreamLine::operator==(const StreamLine &other)
{
    return (steps==other.steps);
}

bool StreamLine::operator!=(const StreamLine &other)
{
    return (steps!=other.steps);
}

bool StreamLine::operator==(int s)
{
    return (steps==s);
}

bool StreamLine::operator!=(int s)
{
    return (steps!=s);
}

int StreamLine::operator*()
{
    return get_ix();
}



