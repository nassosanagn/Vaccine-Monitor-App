#ifndef BLOOM_H_
#define BLOOM_H_

#include <string>
#include <iostream>
#include <cstring>
using namespace std;

class BloomFilter{

    private:
        char *BloomFilterArray;
        int BloomFilterSize;

    public:
        BloomFilter(int size);
        BloomFilter(int size, char* bloomArray);        // a second constructor
        ~BloomFilter();

        int BloomFilterInsert(string str);

        /* Return 1 if answer is "YES" or "MAYBE" and 0 if the answer is "NO" => It's not in the array */
        int BloomFilterSearch(string str);

        void BloomFilterMerge(char* bloomArray);

        char* getBloom(){
            return this->BloomFilterArray;
        }

        int getBloomSize(){
            return this->BloomFilterSize;
        }
};


#endif