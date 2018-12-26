#include <iostream>
#include <fstream>
#include <array>
#include <algorithm>
#include <vector>
#include "PackedArray_my.h"
//#include "kxsort.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <cstdlib>
#include "buffers.h"

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

int main() {
    constexpr unsigned nbuckets_pow = 10;

    rlimit l{0};
    getrlimit(RLIMIT_NOFILE, &l);
    l.rlim_cur = 10 + (1 << nbuckets_pow);
    if (setrlimit(RLIMIT_NOFILE, &l))
        return 1;

    FILE *f = fopen("input", "rb");
    if (!f) return 2;
    FILE *fw = fopen("output", "wb");
    if (!fw) return 3;
//                           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22,23,24
    constexpr unsigned nok[]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 8, 32, 16, 32, 2, 32, 16, 32, 8, 32, 16, 32, 4};
    constexpr unsigned buckets_count = 1 << nbuckets_pow;
    constexpr unsigned bits = 32 - nbuckets_pow;
    constexpr unsigned bucket_nok = nok[bits];

    constexpr unsigned mem = 128 * 1024 * 1024
                             - 64 * 1024 //maintenance
                             - buckets_count * sizeof(size_t) //buckets_sizes
                             - buckets_count * 8 * 1024 //file buffers
    ;

    constexpr unsigned unaligned_bucket_size = mem / buckets_count * 32 / bits / 4; //4848;
    constexpr unsigned max_bucket_size = unaligned_bucket_size - unaligned_bucket_size % bucket_nok;
    constexpr unsigned max_packed_bucket_size = sizeof(PackedArray_my<bits, max_bucket_size>::buf);
    vector<size_t> buckets_sizes(buckets_count, 0);

    vector<FILE *> tmpf(0);
    tmpf.reserve(buckets_count);
    for (int i = 0; i < buckets_count; i++) {
        string fname(to_string(i) + ".tmp");
        FILE *f = fopen(fname.c_str(), "wb+");
        if (!f) {
            cout << "cannot open " << fname << "\n";
            return 4;

        }
        tmpf.push_back(f);
    }

    cout << "packing...\t";

    {
        vector<PackedArray_my<bits, max_bucket_size> > buckets(buckets_count);
        array<uint32_t, 16> readbuf;
        while (!feof(f)) {
            size_t readed = fread(readbuf.data(), 4, readbuf.size(), f);
            for (auto x = readbuf.begin(); x != readbuf.begin() + readed; x++) {
                size_t bucket_index = *x >> (bits);
                auto &bucket = buckets[bucket_index];
                size_t &bucket_size = buckets_sizes[bucket_index];
                bucket.set_or(bucket_size++ % max_bucket_size, *x);
                if (bucket_size % max_bucket_size == 0) {
                    fwrite(bucket.buf, 1, max_packed_bucket_size, tmpf[bucket_index]);
                    bucket.clear();
                }
            }
        }

        for (unsigned bucket_index = 0; bucket_index < buckets_count; bucket_index++) {
            size_t &bucket_size = buckets_sizes[bucket_index];
            if (bucket_size > 0) {
                auto &bucket = buckets[bucket_index];
                fwrite(bucket.buf, 1, bucket.bufsize(bucket_size % max_bucket_size), tmpf[bucket_index]);
            }
        }
    }

    cout << relClockOffset() << "\n";

    cout << "sorting...\t";
    {
        vector<uint64_t> cnt(1 << bits);

        PackedArray_my<bits, max_bucket_size> pa;

        WriteBuf<uint32_t, 32*1024> write_buf(fw);

        for (unsigned bucket_index = 0; bucket_index < buckets_count; bucket_index++) {
            fseek(tmpf[bucket_index], 0, SEEK_SET);

            memset(cnt.data(), 0, cnt.size() * sizeof(cnt[0]));


            auto bucket_size = buckets_sizes[bucket_index];
            vector<uint32_t> kx(0);
            kx.reserve(bucket_size);

            while (bucket_size > 0) {
                unsigned els_count = bucket_size >= max_bucket_size ? max_bucket_size : bucket_size % max_bucket_size;
                unsigned to_read = els_count == max_bucket_size ? max_packed_bucket_size :
                                   PackedArray_my<bits, max_bucket_size>::bufsize(els_count);

                size_t consumed = fread(pa.buf, 1, to_read, tmpf[bucket_index]);
                if (consumed != to_read)
                    return 5;

                for (unsigned i = 0; i < els_count; i++) {
//                    kx.push_back(pa.get(i) ^ (bucket_index << bits)); //radix
                    cnt[pa.get(i)]++; //cnt
//                    cnt[pa.get(i)]++; //stop
                }
                bucket_size -= els_count;
            }

//            kx::radix_sort(kx.begin(), kx.end()); //radix
//            fwrite(kx.data(), 4, kx.size(), fw); //radix

            for (int a = 0; a < 1 << bits; a++) { //cnt
                auto tot = cnt[a];
                for (int i = 0; i < tot; i++) {
                    write_buf.put(a ^(bucket_index << bits));
                }
            }

            write_buf.flush();
        }
    }

    cout << relClockOffset() << "\n";

    fclose(f);
    fclose(fw);
    for (unsigned bucket_index = 0; bucket_index < buckets_count; bucket_index++) {
        fclose(tmpf[bucket_index]);
        string fname(to_string(bucket_index) + ".tmp");
        remove(fname.c_str());
    }

    return 0;

}