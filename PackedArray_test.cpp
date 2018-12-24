#include "gtest/gtest.h"
#include "PackedArray.h"
#include <random>
#include <vector>

using namespace std;

clock_t gClock = clock();
double relClockOffset() {
    clock_t c2 = clock();
    double ret = double(c2 - gClock) / CLOCKS_PER_SEC;
    gClock = c2;
    return ret;
}

TEST(PackedArray, speed) {
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint32_t> distr;

    const unsigned tot = 128*1024*1024;
    const unsigned els = 2048;

    vector<uint32_t> offs;
    offs.reserve(tot);
    vector<uint32_t> val;
    val.reserve(tot);

    for (int a = 0; a < els; a++) {
        val.push_back(distr(eng));
        offs.push_back(distr(eng) & (els-1));
    }

    for (int bits = 1; bits < 32; bits++) {

        PackedArray *pa = PackedArray_create(bits, els);
        for(int i =0; i<tot; i++) {
            PackedArray_set(pa, offs[i], val[i]);
        }

        cerr << "bits="<<bits <<"speed: "<< tot / 1024 / 1024/ relClockOffset() << "MEl"<< "\n";
    }
}

TEST(PackedArray, Random) {
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint32_t> distr;

    for(int i = 1; i<1000;i++)
    for (int bits = 1; bits < 32; bits++) {
//        unsigned bits = 21;
        unsigned els = 32*i;
        PackedArray *pa = PackedArray_create(bits, els);
        vector<uint32_t> orig;
        orig.reserve(els);

        for (int a = 0; a < els; a++) {
            uint32_t x = distr(eng) & ((1 << bits) - 1);
            orig.push_back(x);
        }

//        cerr << "packing...\t";

        PackedArray_pack(pa, 0, orig.data(), els);

//        cerr << relClockOffset() << "\n";

        PackedArray *pa2 = PackedArray_create(bits, els);
        pa2->count = pa->count;
        memcpy(pa2->buffer, pa->buffer, 4 * els * bits / 32); //4*PackedArray_bufferSize(pa));//

//        cerr << "unpacking...\t";
        vector<uint32_t> unpacked(els);
        pa->padding[0] = 0;
        pa->padding[1] = 0;
        PackedArray_unpack(pa2, 0, unpacked.data(), els);
//        cerr << relClockOffset() << "\n";

        for (int i = 0; i < els; ++i) {
            EXPECT_EQ(orig[i], unpacked[i]) << "Vectors differs at index " << i;
            if (orig[i] != unpacked[i]) {
                return;
            }
        }
    }
}


