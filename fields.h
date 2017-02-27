#ifndef FIELDS_H
#define FIELDS_H

#include <algorithm>
#include <rfftw.h>              //the numerical simulation FFTW library

class scalarField
{
protected:
    fftw_real* field;
    fftw_real min, max;
public:
    scalarField();
    void initialize(int); // mimics the simulation class initialization pattern

    void update(int, fftw_real);
    fftw_real read(int);

    fftw_real scale(fftw_real, fftw_real, fftw_real);
};

class vectorialField : public scalarField
{
private:
    scalarField x,y;
public:
    void initialize(int); // also initializes x,y

    void update_x(int, fftw_real);
    void update_y(int, fftw_real);

    fftw_real read_x(int);
    fftw_real read_y(int);
};


#endif // FIELDS_H
