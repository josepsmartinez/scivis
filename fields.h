#ifndef FIELDS_H
#define FIELDS_H

#include <algorithm>
#include <rfftw.h>              //the numerical simulation FFTW library

class Field
{
protected:
    //fftw_real* field;

    fftw_real dim_size;
    fftw_real min, max;

    //void update_grad(int);
public:
    fftw_real* field;

    Field();
    void initialize(int, int); // mimics the simulation class initialization pattern

    void update(int, fftw_real);
    fftw_real read(int);

    fftw_real get_min();
    fftw_real get_max();
    void reset_limits();

    fftw_real scale(fftw_real, fftw_real, fftw_real);
    int index1d(int, int);
};

class vectorialField : public Field
{
private:
    //scalarField x,y;
public:
    Field x,y;
    void initialize(int,int); // also initializes x,y



    void update_x(int, fftw_real);
    void update_y(int, fftw_real);

    fftw_real read_x(int);
    fftw_real read_y(int);
};

class scalarField : public Field
{
protected:
    vectorialField gradient;

public:
    void initialize(int,int); // also initializes gradient
    void update(int, fftw_real); // also updates gradient
};




#endif // FIELDS_H
