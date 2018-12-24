#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace std;

int main(int argc, const char *argv[]) {
    ifstream f("input", std::ios::binary);
    f.seekg(0, ios::end);

    vector<uint32_t > arr;
    arr.reserve(f.tellg()/4);

    f.seekg(0, ios::beg);

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