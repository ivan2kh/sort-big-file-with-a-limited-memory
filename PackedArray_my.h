//
// Created by ivan2kh on 12/22/18.
//

#ifndef SORT_PACKEDARRAY_H
#define SORT_PACKEDARRAY_H



template <unsigned bits, unsigned count>
struct PackedArray_my {
    PackedArray_my():buf() {}
    void clear() {
        memset(buf, 0, sizeof(buf));
    }
    void set_or(unsigned offset, uint32_t x) {
        x <<= 32-bits;

        unsigned cell = offset*bits / 32;
        unsigned start_bit = (offset*bits) % 32;
        if(start_bit <= (32 - bits) ) {
            buf[cell] |= x >> start_bit;
        } else {
            buf[cell] |= x >> start_bit;
            buf[cell + 1] |= x << (32-start_bit);
        }
    }
    uint32_t get(unsigned offset) {
        const uint32_t mask = (1<<bits)-1;
        unsigned cell = offset*bits / 32;
        unsigned start_bit = (offset*bits) % 32;
        if(start_bit <= (32 - bits) ) {
            return mask & (buf[cell] >> (32 - start_bit - bits));
        } else {

//            buf[cell]
//            buf[cell+1]
        }
    }
    uint32_t buf[count*bits/32];
};


#endif //SORT_PACKEDARRAY_H
