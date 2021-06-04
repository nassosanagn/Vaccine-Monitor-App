#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <sstream>
#include <signal.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "ReadFile.h"
#include "bloomList.h"
#include "PipeFunctions.h"

typedef struct{
    pid_t monitorId;
    int readFd;
    int writeFd;
    int socketFd;
    CountryNode* countryListHead = NULL;
}Info;

typedef struct{
    int totalReq = 0;
    int acceptedReq = 0;
    int rejectedReq = 0;
}NumOfRequests;

int Commands(int bloomSize, int numMonitors, int bufferSize, Info monitorInfo[], BloomNode* bloomListHead, NumOfRequests* req);

#endif /* COMMANDS_H_ */