#ifndef PIPE_FUNCTIONS_H_
#define PIPE_FUNCTIONS_H_

#include <iostream>
#include <cstring>
#include <unistd.h>

using namespace std;

/* Functions to read and write strings in named pipes */
string readString(int readFd, int bufferSize);
void writeString(const char* strToSend, int bufferSize, int writeFd);

/* Functions to read and write ints in named pipes */
int readInt(int readFd);
void writeInt(int x, int writeFd);

/* Functions to read and write bloom filters in named pipes */
char* readBloom(int readFd, int bufferSize, int bloomSize);
void writeBloom(const char* strToSend, int bufferSize, int bloomSize, int writeFd);


#endif /* VIRUS_LIST_H_ */