#include "fields.h"

scalarField::scalarField()
{

}

void scalarField::initialize(int N)
{
    field = (fftw_real*)malloc(sizeof(fftw_real) * N);
    for(int i=0; i<N; i++)
        field[i] = 0.f;
    min = max = 0.f;
}

void scalarField::update(int ix, fftw_real value)
{
    field[ix] = value;
    min = std::min(min, value);
    max = std::max(max, value);
}

fftw_real scalarField::read(int ix)
{
    return field[ix];
}

fftw_real scalarField::scale(fftw_real value, fftw_real smin=NULL, fftw_real smax=NULL)
{
    smin = smin == NULL ? min : smin;
    smax = smax == NULL ? max : smax;

    return (value - smin)/(smax - smin);
}

//

void vectorialField::initialize(int N)
{
    scalarField::initialize(N);
    x.initialize(N); y.initialize(N);
}

void vectorialField::update_x(int ix, fftw_real value)
{
    x.update(ix, value);
    update(ix, x.read(ix)*x.read(ix) + y.read(ix)*y.read(ix));
}
void vectorialField::update_y(int ix, fftw_real value)
{
    y.update(ix, value);
    update(ix, x.read(ix)*x.read(ix) + y.read(ix)*y.read(ix));
}

fftw_real vectorialField::read_x(int ix){
    return x.read(ix);
}
fftw_real vectorialField::read_y(int ix){
    return y.read(ix);
}
