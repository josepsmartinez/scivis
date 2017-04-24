#include "timebuffer.h"


void TimeBuffer::initialize(int n, vectorialField *src, int dime)
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
        storage[stored].deepcopyv(source);
        stored++;
    }
    else {
        storage[istart].deepcopyv(source); // overwrites initial position
        istart++;
        if(istart==storage.size()) istart =0;
    }
}

vectorialField* TimeBuffer::read(int i)
{
    if (i < stored) {
        i += istart;
        i = i%storage.size();
        return &(storage[i]);
    }
    else return NULL;
}
