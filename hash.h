#ifndef HASH_H_
#define HASH_H_

/* hash functions cpp */
#include <iostream>
#include <cstring>

unsigned long djb2(unsigned char *str);
unsigned long sdbm(unsigned char *str);
unsigned long hash_i(unsigned char *str, unsigned int i);
int hashedValues(std::string str, int K ,unsigned long hashed[]);

#endif