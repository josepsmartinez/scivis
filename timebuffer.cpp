#include "timebuffer.h"


template <typename F>
void TimeBuffer<F>::initialize(int n, F* src, int dime)
{
    size=n; source=src;
    dim = dime;
    istart=0;
    stored=0;
    storage.resize(n);


}

template <typename F>
void TimeBuffer<F>::append()
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

template <typename F>
F* TimeBuffer<F>::read(int i)
{
    if (i < stored) {
        i += istart;
        i = i%storage.size();
        return &(storage[i]);
    }
    else return NULL;
}


template class TimeBuffer<vectorialField>;
template class TimeBuffer<Field>;
