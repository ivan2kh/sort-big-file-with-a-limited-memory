## sort-big-file-with-a-limited-memory
Sort big file on SSD with a limited memory (128MB)

The program should read unsigned 32 bit integers from "input" file and write sorted numbers to "output" file. 


### Build

You may modify _nbuckets_pow_ constant in _radix.cpp_ in order to choose the number of buckets for radix sort. 
Actual number of buckets number will be **2 pow nbuckets_pow**.
Recommended values are between **8** and **11**. 

```shell
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make radix
```

### Running
There are two stages during the **radix** execution.
1. **Packing** will split input file into buckets using 
MSD (most significant digit in 
[Radix Sort](https://en.wikipedia.org/wiki/Radix_sort) meaning). 
Number of bits in MSD is determined by _nbuckets_pow_ constant 
Intermediate values in memory and in bucket files are stored 
in bit packed form.
2. **Sorting** will sequentially unpack values from bucket files and perform counting sort (uncomment for radix sort) on each bucket. Finally it will store sorted values to output file. 
