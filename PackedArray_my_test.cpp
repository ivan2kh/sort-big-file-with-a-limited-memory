#include "gtest/gtest.h"
#include "PackedArray_my.h"
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

const unsigned tot = 128*1024*1024;
const unsigned els = 2048;

vector<uint32_t> offs(0);
vector<uint32_t> val(0);

template <unsigned bits>
struct TestBits{
    static void test() {

        relClockOffset();

        PackedArray_my<bits, els> pa;

        for(int a=0;a<tot;a++) {
            pa.set_or(offs[a], val[a]);
        }

        cerr << "bits="<<bits <<"speed: "<< tot / 1024 / 1024/ relClockOffset() << "MEl"<< "\n";
    }
};

TEST(PackedArray, Random)
{
    offs.reserve(tot);
    val.reserve(tot);

    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint32_t> distr;

    for (int a = 0; a < els; a++) {
        val.push_back(distr(eng));
        offs.push_back(distr(eng) & (els-1));
    }

    TestBits<16>::test();
    TestBits<17>::test();
    TestBits<18>::test();
    TestBits<19>::test();
    TestBits<20>::test();
    TestBits<21>::test();
    TestBits<22>::test();

}


