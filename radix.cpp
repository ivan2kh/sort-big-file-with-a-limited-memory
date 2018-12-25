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
//      cnt	radix
//22	131	310	10.8593942379
//21	161	287	9.6955006817
//20	239	268	7.915443702
//19	336	264	6.7640692641 <--best performance on my ssd
//18	346	249	6.9062376674
//17	356	238	7.0106694363
//16	460	197	7.2500551755
//Integer packing is almost constant 450 MEls/s
//
#include <sys/time.h>
#include <sys/resource.h>

int main() {
    rlimit l;
    getrlimit(RLIMIT_NOFILE, &l);
    l.rlim_cur = 1<<17;
    setrlimit(RLIMIT_NOFILE, &l);

    FILE *f = fopen("input", "rb");
    if(!f) return -1;
    FILE *fw = fopen("output", "wb");
    if(!fw) return -1;
//                           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22,23
    constexpr unsigned nok[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32, 8,32,16,32, 2,32,16,32, 8,32,16,32};
    constexpr unsigned nbuckets_pow = 11;
    constexpr unsigned buckets_count = 1<<nbuckets_pow;
    constexpr unsigned bits = 32-nbuckets_pow;
    constexpr unsigned bucket_nok = nok[bits];
    constexpr unsigned unaligned_bucket_size = 2048;
    constexpr unsigned max_bucket_size = unaligned_bucket_size - unaligned_bucket_size % bucket_nok;
    constexpr unsigned max_packed_bucket_size = sizeof(PackedArray_my<bits, max_bucket_size>::buf);
    vector<PackedArray_my<bits, max_bucket_size> > buckets(buckets_count);
    vector<size_t> buckets_sizes(buckets_count, 0);
    array<uint32_t, 16> readbuf;

    vector<FILE *> tmpf(0);
    tmpf.reserve(buckets_count);
    for(int i=0;i< buckets_count; i++) {
        string fname("./tmp/" + to_string(i)+".bin");
        FILE *f = fopen(fname.c_str(), "wb+");
        if(!f) {
            cerr << "cannot open "<<fname<<"\n";
            return -1;

        }
        tmpf.push_back(f);
    }

    cerr << "packing...\t";

    while(!feof(f)) {
        size_t readed = fread(readbuf.data(), 4, readbuf.size(), f);
        for(auto x=readbuf.begin();x!=readbuf.begin()+readed; x++) {
            size_t bucket_index = *x >> (bits);
            auto &bucket = buckets[bucket_index];
            size_t &bucket_size = buckets_sizes[bucket_index];
            bucket.set_or(bucket_size++ % max_bucket_size, *x);
            if(bucket_size % max_bucket_size == 0) {
                fwrite(bucket.buf, 1, max_packed_bucket_size, tmpf[bucket_index]);
                bucket.clear();
            }
        }
    }

    for(unsigned bucket_index=0; bucket_index< buckets_count; bucket_index++) {
        size_t &bucket_size = buckets_sizes[bucket_index];
        if(bucket_size>0) {
            auto &bucket = buckets[bucket_index];
            fwrite(bucket.buf, 1, bucket.bufsize(bucket_size % max_bucket_size), tmpf[bucket_index]);
        }
    }

    cerr << relClockOffset() << "\n";

    return 0;
    cerr << "sorting...\t";

    vector<size_t> cnt(1 << bits);

    PackedArray_my<bits, max_bucket_size> pa;

    for(unsigned bucket_index=0; bucket_index< buckets_count; bucket_index++) {
        fseek(tmpf[bucket_index], 0, SEEK_SET);

        memset(cnt.data(), 0, cnt.size() * sizeof(cnt[0]));

        auto bucket_size = buckets_sizes[bucket_index];
        while(bucket_size>0) {
            unsigned els_count = bucket_size>=max_bucket_size ? max_packed_bucket_size : max_bucket_size % max_packed_bucket_size;
            unsigned to_read = els_count == max_bucket_size ? max_packed_bucket_size :
                               PackedArray_my<bits, max_bucket_size>::bufsize(els_count);
            size_t consumed = fread(pa.buf, 1, to_read, tmpf[bucket_index]);
            if(consumed!= to_read)
                return -1;

            for(unsigned i=0;i<els_count;i++) {
                cnt[pa.get(i)]++;
            }
            bucket_size-=els_count;
        }


        for (int a = 0; a < 1 << bits; a++) {
            auto tot = cnt[a];
            for (int i = 0; i < tot; i++) {
                uint32_t val = a^(bucket_index<<bits);
                fwrite(&val, 4, 1, fw);
            }
        }
    }

    cerr << relClockOffset() << "\n";

    fclose(f);
    fclose(fw);
    for(unsigned bucket_index=0; bucket_index< buckets_count; bucket_index++) {
        fclose(tmpf[bucket_index]);
    }

}