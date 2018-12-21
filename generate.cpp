#include <iostream>
#include <stdexcept>
#include <fstream>
#include <random>
#include <string>
using namespace std;


int main(int argc, const char *argv[]){
    size_t sz = stoll(argv[2])/1024;

    ofstream f(argv[1], std::ios::out | std::ios::binary);
    uint32_t arr[1024];

    std::random_device rd;     //Get a random seed from the OS entropy device, or whatever
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<uint32_t> distr;
    for(size_t a=0;a <sz; a++) {
        for(int x=0;x<1024;x++) {
            arr[x] = distr(eng);
        }
        f.write((char*)arr, sizeof(arr));
    }
    f.write((char*)arr, sizeof(arr));
}