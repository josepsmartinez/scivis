#ifndef FIELDS_H
#define FIELDS_H

#include <algorithm>
#include <rfftw.h>              //the numerical simulation FFTW library

class Field
{
protected:
    //fftw_real* field;

    int dim_size;
    fftw_real min, max;

public:
    fftw_real* field;

    Field();
    Field(const Field &f);
    void deepcopy(Field *f);
    void initialize(int, int); // mimics the simulation class initialization pattern

    void update(int, fftw_real);
    void inc(int, fftw_real);
    fftw_real read(int);

    fftw_real get_min();
    fftw_real get_max();
    void reset_limits();

    fftw_real scale(fftw_real, fftw_real, fftw_real);
    int index1d(int, int);

};



// extends base interface to vectorial f=(x,y)
//  where the main read represent the field magnitude |f|
class vectorialField : public Field
{
protected:
    Field x,y;
private:
    //scalarField x,y;
public:
    //gradientField gradient;

    void initialize(int,int); // also initializes x,y


    void deepcopyv(vectorialField *f);
    void update_x(int, fftw_real);
    void update_y(int, fftw_real);
    void inc_x(int, fftw_real);
    void inc_y(int, fftw_real);

    fftw_real read_x(int);
    fftw_real read_y(int);
};

class gradientField : public vectorialField {
private:
protected:
    const fftw_real delta = 1;
    Field* main;
public:
    gradientField(Field* main_field): main(main_field) {}

    void update(int, fftw_real); // updates based on main field
};

class cField : public Field
{
protected:

public:
    cField(): gradient(this) {}

    gradientField gradient;

    void initialize(int,int); // also initializes gradient
    void update(int, fftw_real); // also updates gradient

    gradientField* get_gradient(){return &gradient;}
};

class cVectorialField : public vectorialField {
protected:
    gradientField gradient;
public:
    cVectorialField(): gradient(this) {}

    void initialize(int,int); // also initializes gradient
    void update_x(int, fftw_real); // also updates gradient
    void update_y(int, fftw_real); // also updates gradient


    gradientField* get_gradient(){return &gradient;}
};






#endif // FIELDS_H
