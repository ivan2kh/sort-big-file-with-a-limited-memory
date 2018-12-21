#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, const char *argv[]) {
    ifstream f("input", std::ios::binary);


    vector<uint32_t > arr;
    while(!f.eof()) {
        uint32_t x;
        f.read((char*)&x, sizeof(x));
        if(f) {
            arr.push_back(x);
        }

    }
    sort(arr.begin(), arr.end());

    fstream of(argv[1], std::ios::binary|std::ios::out);
    of.write((char*)arr.data(), arr.size()*4);
    return 0;
}