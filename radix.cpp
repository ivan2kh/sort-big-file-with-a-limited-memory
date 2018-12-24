#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>
#include <array>
#include <algorithm>
#include <vector>
#include "PackedArray.h"
#include "PackedArray_my.h"

using namespace std;

clock_t gClock = clock();
double relClockOffset() {
    clock_t c2 = clock();
    double ret = double(c2 - gClock) / CLOCKS_PER_SEC;
    gClock = c2;
    return ret;
}

int main() {
    FILE *f = fopen("input", "rb");
    if(!f) return -1;
//                           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22,23
    constexpr unsigned nok[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32, 8,32,16,32, 2,32,16,32, 8,32,16,32};
    constexpr unsigned nbuckets_pow = 11;
    constexpr unsigned buckets_count = 1<<nbuckets_pow;
    constexpr unsigned bits = 32-nbuckets_pow;
    constexpr unsigned bucket_nok = nok[bits];
    constexpr unsigned unaligned_bucket_size = 2048;
    const size_t max_bucket_size = unaligned_bucket_size - unaligned_bucket_size % bucket_nok;
    vector<array<uint32_t, max_bucket_size> > buckets(buckets_count);
    vector<size_t> buckets_sizes(buckets_count, 0);
    array<uint32_t, 16> readbuf;

    PackedArray *pa = PackedArray_create(32-nbuckets_pow, max_bucket_size);

    vector<ofstream> of(0);
    for(int i=0;i< buckets_count; i++) {
        of.emplace_back("./tmp/" + to_string(i)+".bin", ios::binary|ios::trunc|ios::in|ios::out);
    }

    cerr << "packing...\t";

    while(!feof(f)) {
        size_t readed = fread(readbuf.data(), 4, readbuf.size(), f);
        for(auto x=readbuf.begin();x!=readbuf.begin()+readed; x++) {
            size_t bucket_index = *x >> (bits);
            auto &bucket = buckets[bucket_index];
            size_t &bucket_size = buckets_sizes[bucket_index];
            bucket[bucket_size++ % max_bucket_size] = *x;
            if(bucket_size % max_bucket_size == 0) {
//                of[bucket_index].write((char*)bucket.data(), 4*max_bucket_size);
                PackedArray_pack(pa, 0, bucket.data(), max_bucket_size);
                of[bucket_index].write((char*)pa->buffer, 4 * max_bucket_size * bits / 32);
            }
        }
    }

    for(unsigned bucket_index=0; bucket_index< buckets_count; bucket_index++) {
        size_t &bucket_size = buckets_sizes[bucket_index];
        if(bucket_size>0) {
            auto &bucket = buckets[bucket_index];
            pa->count = bucket_size % max_bucket_size;
            PackedArray_pack(pa, 0, bucket.data(), bucket_size % max_bucket_size);
            of[bucket_index].write((char *) pa->buffer, 4 * PackedArray_bufferSize(pa));
        }
    }

    cerr << relClockOffset() << "\n";


}