#ifndef PIPE_FUNCTIONS_H_
#define PIPE_FUNCTIONS_H_

#include <iostream>
#include <cstring>
#include <unistd.h>

#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

using namespace std;

/* Functions to read and write string in sockets */
string readString(int socketFd, int bufferSize);
int sendString(int socketFd, const char* strToSend, int bufferSize);

/* Functions to read and write ints in sockets */
int readInt(int socketFd);
void sendInt(int socketFd, int x);

/* Functions to read and write bloom filters in sockets */
char* readBloom(int socketFd, int bloomSize, int bufferSize);
void sendBloom(int socketFd, const char* strToSend, int bloomSize, int bufferSize);

#endif /* VIRUS_LIST_H_ */