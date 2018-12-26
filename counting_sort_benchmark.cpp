//
// Created by ivan2kh on 12/23/18.
//
#include <random>
#include <vector>
#include "kxsort.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "buffers.h"

using namespace std;

clock_t gClock = clock();
double relClockOffset() {
    clock_t c2 = clock();
    double ret = double(c2 - gClock) / CLOCKS_PER_SEC;
    gClock = c2;
    return ret;
}

void writeFilePerformance() {
    FILE *fw = fopen("output", "wb");

    std::mt19937_64 eng(42);
    std::uniform_int_distribution <uint32_t> distr;

    const unsigned bits = 23;
    const unsigned bucket_index = 239;
    uint32_t mask = (unsigned)(1 << bits) - 1;
    unsigned els = 128 * 1024 * 1024 / 4;
    vector<uint32_t> kx;
    kx.reserve(els);

    for (int a = 0; a < els; a++) {
        kx.push_back(distr(eng) & mask);
    }

    vector<unsigned> cnt(1 << bits);

    vector<uint32_t> cnt_out;
    cnt_out.reserve(els);

    relClockOffset();

    memset(cnt.data(), 0, cnt.size() * sizeof(cnt[0]));

    for (auto x:kx) { //cnt
        cnt[x]++;
    }


    for (int a = 0; a < 1 << bits; a++) { //cnt
        auto tot = cnt[a];
        for (int i = 0; i < tot; i++) {
            cnt_out.push_back(a ^(bucket_index << bits));
        }
    }
    fwrite(cnt_out.data(), 4, cnt_out.size(), fw); //radix

    cerr << "\tCNT desired" << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
         << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s" << "\n";

    relClockOffset();
    WriteBuf<uint32_t, 32*1024> buf(fw);

    memset(cnt.data(), 0, cnt.size() * sizeof(cnt[0]));

    for (auto x:kx) { //cnt
        cnt[x]++;
    }


    for (int a = 0; a < 1 << bits; a++) { //cnt
        auto tot = cnt[a];
        for (int i = 0; i < tot; i++) {
            buf.put(a ^(bucket_index << bits));
        }
    }

    buf.flush();

    cerr << "\tCNT+buf " << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
         << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s" << "\n";

    relClockOffset();

    kx::radix_sort(kx.begin(), kx.end()); //radix
    fwrite(kx.data(), 4, kx.size(), fw); //radix

    cerr << "\tKX  " << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
         << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n";
}

void bitsPerElementVSbucketSize() {
    std::mt19937 eng(42);
    std::uniform_int_distribution <uint32_t> distr;

    for(unsigned bpe = 19; bpe<=23; bpe++) {
        uint32_t mask = (1 << bpe) - 1;
        for (int i = 1; i <= 128; i <<= 1) {
            unsigned els = i * 1024 * 1024 / 4;
            vector<uint32_t> orig;
            orig.reserve(els);

            for (int a = 0; a < els; a++) {
                orig.push_back(distr(eng) & mask);
            }

            vector<uint32_t> copy(orig);


            vector<unsigned> cnt(1 << bpe, 0);
            relClockOffset();

            for (auto x: orig) {
                cnt[x]++;
            }

            copy.resize(0);

            for (int a = 0; a < 1 << bpe; a++) {
                auto tot = cnt[a];
                for (int i = 0; i < tot; i++) {
                    copy.push_back(a);
                }
            }
            cerr << "bits " << bpe<<" ";
//            cerr << "\tCNT " << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
//                 << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s" << "\n";
            cerr << " CNT " << els * 4 / 1024.0 / 1024.0 << " MB chunk : "
                 << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " ";

            //Radix Sort
            copy = orig;
            relClockOffset();
            kx::radix_sort(copy.begin(), copy.end());
//            cerr << "\tKX  " << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
//                 << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n";
            cerr << " KX  " << els * 4 / 1024.0 / 1024.0 / relClockOffset() << "\n";

//            copy = orig;
//            relClockOffset();
//
//            sort(copy.begin(), copy.end());
//
//            cerr << "\tSTL " << els * 4 / 1024.0 / 1024.0 << "MB chunk : \t"
//                 << els * 4 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n";
        }
    }

}

int main() {
//    writeFilePerformance();
    bitsPerElementVSbucketSize();
    return 0;
    std::mt19937 eng(42);
    std::uniform_int_distribution <uint16_t> distr;

    for(int i=1; i<=128; i<<=1) {
        unsigned els = i * 1024 * 1024 / 4;
        vector<uint16_t> orig;
        orig.reserve(els);

        for (int a = 0; a < els; a++) {
            orig.push_back(distr(eng));
        }

        vector<uint16_t> copy(orig);

        relClockOffset();
//        cerr << "radix...";
//        cerr <<"sorting...\n";
        kx::radix_sort(copy.begin(), copy.end());
//        cerr<<relClockOffset();
        cerr << "kx " << els * 2 / 1024.0 / 1024.0 << "MB chunk : \t" << els * 2 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n";

        copy = orig;
        vector<unsigned> cnt(65536,0);
        relClockOffset();

        for(auto x: orig) {
            cnt[x]++;
        }

        copy.resize(0);

        for(int a=0;a<65536;a++) {
            auto tot = cnt[a];
            for(int i=0;i<tot;i++) {
                copy.push_back(a);
            }
        }

        cerr << "counting " << els * 2 / 1024.0 / 1024.0 << "MB chunk : \t" << els * 2 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n";

//        copy = orig;
//        relClockOffset();
//
//        cerr << "stl sort...";
//
//        sort(copy.begin(), copy.end());
//
//        cerr << "stl " << els * 2 / 1024.0 / 1024.0 << "MB chunk : \t" << els * 2 / 1024.0 / 1024.0 / relClockOffset() << " mb/s"<< "\n\n";
    }
}