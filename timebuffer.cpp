#include "timebuffer.h"



void TimeBuffer::initialize(int n, Field *src)
{
    size=n; source=src;
    istart=0;
    storage = vector<Field>(n);


}

void TimeBuffer::append()
{
    if (stored < storage.size()) { // buffer is not full yet

        storage.push_back(Field(*source));
    }
    else {
        storage[istart] = Field(*source); // overwrites initial position
        istart++; stored++;
    }
}

Field* TimeBuffer::read(int i)
{
    if (i < stored) {
        i += istart;
        return &(storage[i]);
    }
    else return NULL;
}
