#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>
#include <array>
#include <algorithm>
#include <vector>

using namespace std;

clock_t gClock = clock();
double relClockOffset() {
    clock_t c2 = clock();
    double ret = double(c2 - gClock) / CLOCKS_PER_SEC;
    gClock = c2;
    return ret;
}

int main(){
    ifstream f("input", std::ios::binary);

    cerr << "detect pool sizes...\t";

    const size_t bits_per_pool = 16;
    const size_t pools_count = 2<<(32-bits_per_pool);
    vector<uint64_t> poolsize(pools_count, 0);

    while(!f.eof()) {
        uint32_t x;
        f.read((char*)&x, sizeof(x));
        if(f) {
            poolsize[x >> bits_per_pool]++;
        }
    }

    vector<uint64_t> offsets;
    offsets.reserve(pools_count);

    uint64_t sum=0;
    for(auto x: poolsize) {
        offsets.push_back(sum);
        sum+= x*4;
    }

    vector<uint64_t> offsets2(offsets);

    cerr << relClockOffset() << "\n";
    cerr << "write unsorted...\t";

    const uint64_t mem = 128'000'000 -
            pools_count*8 /*poolsize*/ -
            2*pools_count*4 /*offsets*/;

    //STAGE 2

    fstream of("output", std::ios::binary | std::ios::out | std::ios::in | std::ios::trunc);

    {
        vector<uint8_t> wrt_offset(pools_count, 0);

        const uint64_t mem2 = mem - pools_count*1 /*wrt_offset*/;

        const size_t max_line = mem2/pools_count/4; //256;
        vector<array<uint32_t, max_line> > cache;
        cache.resize(pools_count);

        f.clear();
        f.seekg(0, ios::beg);

        while (!f.eof()) {
            uint32_t x;
            f.read((char *) &x, sizeof(x));
            if (f) {
                size_t pool = x >> bits_per_pool;

                auto &cl = cache[pool];
                uint8_t &offs = wrt_offset[pool];
                if (offs == numeric_limits<uint8_t>::max()) {
                    cl[offs] = x;
                    of.seekp(offsets[pool]);
                    of.write((char *) &cl, 4 * max_line);
                    offsets[pool] += 4 * max_line;
                    offs = 0;
                } else {
                    cl[offs++] = x;
                }
            }
        }

        cerr << relClockOffset() << "\n";
        cerr << "write remaining pools...\t";

        for (int p = 0; p < pools_count; p++) {
            uint8_t offs = wrt_offset[p];
            if (offs > 0) {
                auto &cl = cache[p];
                of.seekp(offsets[p]);
                of.write((char *) &cl, 4 * (offs));
            }
        }
    }

    cerr << relClockOffset() << "\n";
    cerr << "sort separate pools...\t";

    //STAGE 3

    offsets = offsets2;
//    offsets2.clear();
//    offsets2.shrink_to_fit();
//    const size_t mem3 = mem - pools_count*4 /*offsets*/;

    for(size_t p =0;p<pools_count;p++) {
        if(poolsize[p] < 2<<bits_per_pool) {
            vector<uint32_t > arr(0);
            arr.resize(poolsize[p]);
            of.seekg(offsets[p], ios::beg);
            of.read((char*)arr.data(), arr.size()*4);
            sort(arr.begin(), arr.end());
            of.seekp(offsets[p], ios::beg);
            of.write((char*)arr.data(), arr.size()*4);
        } else {
            vector<uint64_t> cntsrt(2 << bits_per_pool, 0);
            of.seekg(offsets[p], ios::beg);
            size_t cnt = poolsize[p];
            for (int i = 0; i < cnt; i++) {
                uint32_t x;
                of.read((char *) &x, 4);
                cntsrt[x & ((2<<bits_per_pool)-1) ]++;
            }
            of.seekp(offsets[p], ios::beg);
            for (int i = 0; i < 2 << bits_per_pool; i++) {
                uint32_t x = (p << bits_per_pool) | i;
                for (int z = 0; z < cntsrt[i]; z++) {
                    of.write((char *) &x, 4);
                }
            }
        }
    }

    cerr << relClockOffset() << "\n";

    of.close();
}