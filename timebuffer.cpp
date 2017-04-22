#include "timebuffer.h"


void TimeBuffer::initialize(int n, Field *src, int dime)
{
    size=n; source=src;
    dim = dime;
    istart=0;
    stored=0;
    storage.resize(n);


}

void TimeBuffer::append()
{
    if (stored < size) { // buffer is not full yet

        storage[stored].initialize(dim*dim,dim);
        storage[stored].deepcopy(source);
        stored++;
    }
    else {
        storage[istart].deepcopy(source); // overwrites initial position
        istart++;
        if(istart==storage.size()) istart =0;
    }
}

Field* TimeBuffer::read(int i)
{
    if (i < stored) {
        i += istart;
        i = i%storage.size();
        return &(storage[i]);
    }
    else return NULL;
}
