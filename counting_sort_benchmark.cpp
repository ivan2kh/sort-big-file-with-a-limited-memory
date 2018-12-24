//
// Created by ivan2kh on 12/23/18.
//
#include <random>
#include <vector>
#include "kxsort.h"
#include <iostream>
#include <unistd.h>

using namespace std;

clock_t gClock = clock();
double relClockOffset() {
    clock_t c2 = clock();
    double ret = double(c2 - gClock) / CLOCKS_PER_SEC;
    gClock = c2;
    return ret;
}

void gt16() {
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution <uint32_t> distr;

    for(unsigned bpe = 16; bpe<31; bpe++) {
        uint32_t mask = (1 << bpe) - 1;
        for (int i = 1; i <= 128; i <<= 1) {
            unsigned els = i * 1024 * 1024 / 4;
            vector<uint32_t> orig;
            orig.reserve(els);

            for (int a = 0; a < els; a++) {
                orig.push_back(distr(eng) & mask);
            }

            vector<uint32_t> copy(orig);

            copy = orig;
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

            cerr << "bits " << bpe << ". " << els * 2 / 1024.0 / 1024.0 << "MB chunk : \t"
                 << els * 2 / 1024.0 / 1024.0 / relClockOffset() << " mb/s" << "\n";

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

}

int main() {
    gt16();
    return 0;
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
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