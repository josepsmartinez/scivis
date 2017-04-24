#ifndef TIMEBUFFER_H
#define TIMEBUFFER_H

#include "fields.h"

using namespace std;

template <typename F>
class TimeBuffer
{
private:
    int size;
    int dim;
    F* source;
    int istart, stored;
    vector<F> storage;

public:
    TimeBuffer<F>() {}
    ~TimeBuffer<F>() {}
    void initialize(int n, F* src, int dim);
    //void initialize(int n, Field *src, int dim);

    void append();
    F* read(int i);
};

#endif // TIMEBUFFER_H
