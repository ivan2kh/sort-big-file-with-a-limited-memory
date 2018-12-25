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

const unsigned els = 1<<28;
vector<uint32_t> orig(0);

template <int bpe>
static void test() {
    const unsigned streams = 1<<bpe;
    unsigned mask = streams - 1;

    relClockOffset();

    vector <ofstream> of(0);
    for (int i = 0; i < streams; i++) {
        of.emplace_back("./tmp/" + to_string(i) + ".bin", ios::binary | ios::trunc | ios::in | ios::out);
    }


    constexpr size_t max_bucket_size = 128*1024*1024 / streams * 32 / (32-bpe) / 4;
    vector<array<uint16_t, max_bucket_size> > buckets(streams);
    vector<size_t> buckets_sizes(streams, 0);

    char buf[max_bucket_size*4];

    int i;
    for (unsigned x:orig) {
        size_t bucket_index = x & mask;
        auto &bucket = buckets[bucket_index];
        size_t &bucket_size = buckets_sizes[bucket_index];
        bucket[bucket_size++ % max_bucket_size] = x>>16;
        buf[++i % max_bucket_size*4] = x&mask;
        if (bucket_size % max_bucket_size == 0) {
            of[bucket_index].write(buf, max_bucket_size*4*(32-bpe)/32);
        }
    }

    cerr << "bpe" << bpe << ". "<< els*4/1024/1024 / relClockOffset() << " mb/s"<< "\n";

}

int main()
{
    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution <uint32_t> distr;

    orig.reserve(els);

    for (int a = 0; a < els; a++) {
        orig.push_back(distr(eng));
    }

    test<8>();
    test<10>();
    test<11>();
    test<12>();
    test<13>();
    test<14>();
    test<15>();
    test<16>();
}