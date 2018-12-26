//
// Created by ivan2kh on 12/26/18.
//

#ifndef SORT_BUFFERS_H
#define SORT_BUFFERS_H
#include <cstdio>


template<typename T, unsigned size>
struct WriteBuf {
    explicit WriteBuf(FILE *out):
            buf(new T[size]),
            i(0),
            f(out)
    {}
    ~WriteBuf() {
        delete[] buf;
    }
    void put(T value) {
        buf[i++] = value;
        if(i==size) {
            fwrite(buf, sizeof(T), size, f);
            i=0;
        }
    }

    void flush() {
        if(i==0)
            return;
        fwrite(buf, sizeof(T), i, f);
        i=0;
    }

    unsigned i;
    T *buf;
    FILE *f;
};

#endif //SORT_BUFFERS_H
