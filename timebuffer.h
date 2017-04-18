#ifndef TIMEBUFFER_H
#define TIMEBUFFER_H

#include "fields.h"

using namespace std;
class TimeBuffer
{
private:
    int size;
    Field* source;
    int istart, stored;
    vector<Field> storage;

public:
    TimeBuffer() {};
    ~TimeBuffer() {};
    void initialize(int n, Field* src);

    void append();
    Field* read(int i);
};

#endif // TIMEBUFFER_H
