#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "BloomFilter.h"
#include "CountryList.h"
#include "bloomList.h"
#include "Commands.h"

#include <pthread.h>
#include <stdio.h>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <netdb.h> /* gethostbyaddr */

#define PORT 9000

using namespace std;

struct BloomNode* bloomListHead = NULL;        /* List with a bloom filter for each virus */
struct CountryNode* countryListHead = NULL;                /* One list with all the countries */

int numMonitors;
string* paths;
Info* monitorInfo;                      /* Info array with read file descriptor , write fd and monitor id */

int socketBufferSize = 0;
int sizeOfBloom = 0;
string inputDir;

NumOfRequests req;

int main(int argc, char *argv[]){

    if (argc != 13){
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    int cyclicBufferSize = 0;
    int numThreads = 0;
    
    for (int i = 1; i < argc; i+=2){           /* Get all the arguments */

        if (strcmp(argv[i],"-m") == 0)              /* Get the number of monitors */
            numMonitors = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-b") == 0)               /* Get the size of the socketBuffer */
            socketBufferSize = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-c") == 0)                   /* Get the size of the CyclicBuffer */
            cyclicBufferSize = stoi(argv[i+1]);

        else if (strcmp(argv[i],"-s") == 0)                   /* Get the size of the bloom */
            sizeOfBloom = stoi(argv[i+1]);

        else if (strcmp(argv[i],"-i") == 0)                   /* Get the input directory */
            inputDir = argv[i+1];
        
        else if (strcmp(argv[i],"-t") == 0)                   /* Get the number of Threads */
            numThreads = stoi(argv[i+1]);
    
        else{
           perror("Error getting the arguments");
           exit(EXIT_FAILURE);
        } 
    }

    paths = new string[2 * numMonitors];
    monitorInfo  = new Info[numMonitors];              /* Info array with read file descriptor , write fd and monitor id */

    /* Open directory */
    DIR *inputDIR = opendir(inputDir.c_str());
    if (ENOENT == errno) {
        perror("Couldn't open input directory");
        exit(EXIT_FAILURE);
    }

    struct dirent *counter;
    int countCountries = 0;

    while ((counter = readdir(inputDIR)) != NULL){    
        
        if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) 
            continue;
    
        string directory = counter->d_name;

        /* Create the list with the countries */
        if (!(CountryListSearch(countryListHead, directory))){     /* If country is not already in the list => add it (in the beginning) */
          CountryListPush(&countryListHead, directory);
        }

        CountryListPush(&(monitorInfo[countCountries].countryListHead), directory);
        countCountries++;

        if (countCountries == numMonitors)
            countCountries = 0;
    }
    rewinddir(inputDIR);
    closedir(inputDIR);     /* close the input directory */

    int pid;
    for (int i = 0; i < 2 * numMonitors; i+=2){      /* fork() numMonitors child processes */

        if ((pid = fork()) == 0){
            
            string allCountries = getAllCountries(monitorInfo[i/2].countryListHead);
            cout << "Oles oi xwres einai: " << allCountries.c_str() << endl; 

            string tempPort = to_string((PORT + i/2));
            string tempNumThreads = to_string(numThreads);
            string tempSocketBufferSize = to_string(socketBufferSize);
            string tempCyclicBufferSize = to_string(cyclicBufferSize);
            string tempSizeOfBloom = to_string(sizeOfBloom);

            const char* args[] = {"./monitorServer", "-p", tempPort.c_str(), "-t", tempNumThreads.c_str(), "-b", tempSocketBufferSize.c_str(), "-c", tempCyclicBufferSize.c_str(),
            "-s", tempSizeOfBloom.c_str() , allCountries.c_str() , NULL};

            if (execvp("./monitorServer", (char* const*) args) == -1){
                cout << "Failed execvp!" << endl;
                return 0;
            }
            exit(0);

        }else{
            monitorInfo[i/2].monitorId = pid;
        }
    }
       
    /* ------------------------------ SOCKETS ------------------------------ */

    for (int i = 0; i < numMonitors; i++){

        int sock = 0;
        int connectStatus;
        struct sockaddr_in serverAddr;
        char hostname[1024];
        struct hostent* h;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("\n Socket creation error \n");
            return -1;
        }

        hostname[1023] = '\0';
        gethostname(hostname, 1023);
        h = gethostbyname(hostname);
        //printf("Hostname: %s\n", hostname);
        //printf("h_name: %s\n", h->h_name);

        memset(&serverAddr, '\0', sizeof(serverAddr));
        memcpy(&serverAddr.sin_addr, h->h_addr, h->h_length);

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons((PORT+i));

        do
            connectStatus = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        while (connectStatus < 0);

        monitorInfo[i].socketFd = sock;
    }

    /* ---------------------------------------------------------------- */

    for (int i = 0; i < numMonitors; i++)
        sendString(monitorInfo[i].socketFd, inputDir.c_str(), socketBufferSize);      /* Send the input directory */

    /* --------------------------------------------------------------------------------------------*/

    /* Read the bloom filters */
    fd_set readFdsSet;
    string messageRead;

    FD_ZERO(&readFdsSet);
    for(int i = 0; i < numMonitors; i++){
        FD_SET(monitorInfo[i].socketFd, &readFdsSet);
    }
    
    int maxfd = monitorInfo[0].socketFd;
    for (int i = 1; i < numMonitors; i++){
        if (monitorInfo[i].socketFd > maxfd){
            maxfd = monitorInfo[i].socketFd;
        }
    }

    char* tempBloom;
    int counterMonitor = 0;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    do{
        select(maxfd + 1, &readFdsSet, nullptr, nullptr, &timeout);

        for (int i = 0; i < numMonitors; i++){                   /* For each child */
            
            FD_SET(monitorInfo[i].socketFd, &readFdsSet);

            if (FD_ISSET(monitorInfo[i].socketFd, &readFdsSet)){

                /* Get the number of viruses (equals with the number of bloom filters) */
                int numOfViruses = readInt(monitorInfo[i].socketFd);

                for (int j = 0; j < numOfViruses; j++){                                         /* How many bloom filters to read */

                    string virusNameStr = readString(monitorInfo[i].socketFd, socketBufferSize);        /* Get the virus name */
                    tempBloom = readBloom(monitorInfo[i].socketFd, sizeOfBloom, socketBufferSize);

                    /* Create the list with the bloom filters */
                    if (!(BloomListSearch(bloomListHead, virusNameStr)))                        /* If virusName is not already in the list => add it (in the beginning) */
                        BloomListPush(&bloomListHead, virusNameStr, tempBloom, sizeOfBloom);
                    else
                        BloomListMergeBlooms(bloomListHead, virusNameStr, tempBloom);

                    delete tempBloom;
                }
                counterMonitor++;
                FD_ZERO(&readFdsSet);
            }
        }
        
    }while(counterMonitor != numMonitors);
    

    /* ------------------------------------------ User's commands ------------------------------------------ */

    do{   /* Display the menu with the user commands */

    }while (!Commands(sizeOfBloom, numMonitors, socketBufferSize, monitorInfo, bloomListHead, &req));

    //kill(getpid(), SIGINT);
    cout << "Program completed successfully." << endl;

    /* Wait for each child to finish */
    for (int i = 0; i < numMonitors; i++)
        waitpid(monitorInfo[i].monitorId, NULL, 0);
    
    /* Unlink the named pipes */
    for (int i = 0; i < 2 * numMonitors; i++){
        close(monitorInfo[i].socketFd);
    }

    // string logFileStr = "log_file." + to_string(getpid()) + ".txt";
    // ofstream logFile;
    // logFile.open(logFileStr);                                           /* Create and open the file */

    // CountryListPrintInFile(countryListHead,logFile);                    /* Print all the countries in the logFile */
    // logFile << "TOTAL TRAVEL REQUESTS " << req.totalReq << endl;        /* Print total travel requests in the logfile */
    // logFile << "ACCEPTED " << req.acceptedReq << endl;                  /* Print accepted requests in the logfile */
    // logFile << "REJECTED " << req.rejectedReq << endl;                  /* Print rejected requests in the logfile */

    // logFile.close();                                                    /* Close the file */

    return 0;
}