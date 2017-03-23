#include "fields.h"



Field::Field()
{

}

void Field::initialize(int N, int DIM)
{
    dim_size = DIM;

    field = (fftw_real*)malloc(sizeof(fftw_real) * N);
    for(int i=0; i<N; i++)
        field[i] = 0.f;

    reset_limits();
}



void Field::update(int ix, fftw_real value)
{
    field[ix] = value;
    min = std::min(min, value);
    max = std::max(max, value);
}

fftw_real Field::read(int ix)
{
    return field[ix];
}

fftw_real Field::get_min()
{
    return min;
}
fftw_real Field::get_max(){
    return max;
}

void Field::reset_limits()
{
    min = max = 0.f;
}

fftw_real Field::scale(fftw_real value, fftw_real smin=NULL, fftw_real smax=NULL)
{
    smin = smin == NULL ? min : smin;
    smax = smax == NULL ? max : smax;

    return (value - smin)/(smax - smin);
}

int Field::index1d(int i, int j) {
    return j*50 + i;
}

// SCALAR

void scalarField::initialize(int N, int DIM)
{
    Field::initialize(N, DIM);
    gradient.initialize(N, DIM);
}

void scalarField::update(int ix, fftw_real value)
{
    Field::update(ix, value);
    // updates gradient field

    //if()
}

// VECTORIAL

void vectorialField::initialize(int N, int DIM)
{
    Field::initialize(N, DIM);
    x.initialize(N, DIM); y.initialize(N, DIM);
}

void vectorialField::update_x(int ix, fftw_real value)
{
    x.update(ix, value);
    update(ix, sqrt(x.read(ix)*x.read(ix) + y.read(ix)*y.read(ix)));
}
void vectorialField::update_y(int ix, fftw_real value)
{
    y.update(ix, value);
    update(ix, sqrt(x.read(ix)*x.read(ix) + y.read(ix)*y.read(ix)));
}

fftw_real vectorialField::read_x(int ix){
    return x.read(ix);
}
fftw_real vectorialField::read_y(int ix){
    return y.read(ix);
}

