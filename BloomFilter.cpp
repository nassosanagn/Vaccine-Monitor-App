#include "BloomFilter.h"
#include "hash.h"

#define K 16
#define BYTE_IN_BITS 8

BloomFilter::BloomFilter(int size) {

  this->BloomFilterSize = size;
  this->BloomFilterArray = new char[this->BloomFilterSize];

  memset(this->BloomFilterArray,'\0', BloomFilterSize);       /* Clear the array and set all bits to 0 */
}

BloomFilter::BloomFilter(int size, char* bloomArray){  

  this->BloomFilterSize = size;

  this->BloomFilterArray = new char[size];
  memset(this->BloomFilterArray,'\0', size);       /* Clear the array and set all bits to 0 */  

  /* bitwise or the 2 arrays */
  for (int i = 0; i < size; i++)
    this->BloomFilterArray[i] = (char)(bloomArray[i] | this->BloomFilterArray[i]);
}

void BloomFilter::BloomFilterMerge(char* bloomArray){

  /* bitwise or the 2 arrays */
  for (int i = 0; i < this->BloomFilterSize ; i++)
    this->BloomFilterArray[i] = (char)(bloomArray[i] | this->BloomFilterArray[i]);
} 

BloomFilter::~BloomFilter(){
  delete BloomFilterArray;
}

int BloomFilter::BloomFilterInsert(string str){

  int pos;                            /* position in the array [BloomFilter] */
  int offset;                         /* find the bit to change in that position in [BloomFilter] */
  unsigned long hashed[K];           /* An array with K hashed values */
  
  hashedValues(str, K ,hashed);

  for (int i = 0 ; i < K ; i++){
    /* hashed values mod M = the number of bits in the array that we need to change */
    hashed[i] = hashed[i] % (BloomFilterSize * BYTE_IN_BITS);      /* mod the number of bits in the array */
  }

  /* Insert in BloomFilter */
  for (int i = 0 ; i < K ; i++){
    pos = hashed[i] / BYTE_IN_BITS;                                                /* find the right byte */
    offset = pos % BYTE_IN_BITS;                                                   /* find the right bit */
    BloomFilterArray[pos] =  (BloomFilterArray[pos] | (1 << offset));
  }

  return 1;
}

/* Return 1 if answer is "YES" or "MAYBE" and 0 if the answer is "NO" => It's not in the array */
int BloomFilter::BloomFilterSearch(string str){

  int pos;                            /* position in the array [BloomFilter] */
  int offset;                         /* find the bit to change in that position in [BloomFilter] */
  unsigned long hashed[K];           /* An array with K hashed values */
  
  hashedValues(str, K ,hashed);
   
  for (int i = 0 ; i < K ; i++){
    
    /* hashed values mod M = the number of bits in the array that we need to change */
    hashed[i] = hashed[i] % (BloomFilterSize * BYTE_IN_BITS);      /* mod the number of bits in the array */
  }

  /* Insert in BloomFilter */
  for (int i = 0 ; i < K ; i++){

    pos = hashed[i] / BYTE_IN_BITS;                                     /* find the right byte */
    offset = pos % BYTE_IN_BITS;                                        /* find the right bit */

    /* Check if bit is already 1 */
    char c = (BloomFilterArray[pos] >> offset) & 1;

    if (c != 1){                        /* If bit is already set to 1 then go to the next iteration */
      return 0;
    }
  }

  return 1;
}

