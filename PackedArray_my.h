//
// Created by ivan2kh on 12/22/18.
//

#ifndef SORT_PACKEDARRAY_H
#define SORT_PACKEDARRAY_H

#include <cstring>
#include <cstdint>

template <unsigned bits, unsigned count>
struct PackedArray_my {
    PackedArray_my():buf() {}
    void clear() {
        memset(buf, 0, sizeof(buf));
    }
    void set_or(unsigned offset, uint32_t x) {
        x &= mask;

        unsigned cell = offset*bits / 32;
        unsigned start_bit = (offset*bits) % 32;
        if(start_bit <= (32 - bits) ) {
            buf[cell] |= x << start_bit;
        } else {
            uint64_t &p = (uint64_t&)buf[cell];
            p |= uint64_t(x) << start_bit;
        }
    }
    uint32_t get(unsigned offset) {
        unsigned cell = offset*bits / 32;
        unsigned start_bit = (offset*bits) % 32;
        if(start_bit <= (32 - bits) ) {
            return mask & (buf[cell] >> start_bit);
        } else {
            uint64_t &x = (uint64_t&)buf[cell];
            return mask & (x >> start_bit);
        }
    }
    static unsigned bufsize(unsigned elements) {
        return  4*(elements * bits / 32 + ((elements*bits) % 32 !=0));
    }
    uint32_t buf[count*bits/32];
    const uint32_t mask = (uint32_t)(1<<bits)-1;
};


#endif //SORT_PACKEDARRAY_H
