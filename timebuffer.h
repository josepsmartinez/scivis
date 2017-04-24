#ifndef TIMEBUFFER_H
#define TIMEBUFFER_H

#include "fields.h"

using namespace std;
class TimeBuffer
{
private:
    int size;
    int dim;
    vectorialField* source;
    int istart, stored;
    vector<vectorialField> storage;

public:
    TimeBuffer() {};
    ~TimeBuffer() {};
    void initialize(int n, vectorialField *src, int dim);

    void append();
    vectorialField *read(int i);
};

#endif // TIMEBUFFER_H
