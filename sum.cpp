#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>
#include <array>
#include <algorithm>
#include <vector>
#include <numeric>

using namespace std;

int main(int argc, const char *argv[]) {
    ifstream f(argv[1], std::ios::binary);


    const size_t sz = 128*1024*1024;

    vector<uint32_t> arr(sz);

    uint64_t sum=0;
    while (!f.eof()) {
        f.read((char *)arr.data(), arr.size()*4);
        sum += accumulate(arr.begin(),arr.end(), uint64_t(0));
    }
    cout<<sum;

}