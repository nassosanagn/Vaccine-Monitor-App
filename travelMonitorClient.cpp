#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "BloomFilter.h"
#include "CountryList.h"
#include "bloomList.h"
#include "Commands.h"

#include <stdio.h>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

using namespace std;

struct BloomNode* bloomListHead = NULL;        /* List with a bloom filter for each virus */
struct CountryNode* countryListHead = NULL;                /* One list with all the countries */

bool suicide = false;
int numMonitors;
string* paths;
Info* monitorInfo;                      /* Info array with read file descriptor , write fd and monitor id */

int socketBufferSize = 0;
int sizeOfBloom = 0;
string inputDir;

NumOfRequests req;

void handle_sigint_parent(int sig);
void handle_SIGCHLD_parent(int sig);

int main(int argc, char *argv[]){

    if (argc != 13){
        perror("Invalid number of arguments");
        exit(1);
    }

    int cyclicBufferSize = 0;
    int numThreads = 0;
    
    for (int i = 1; i < argc; i = i + 2){           /* Get all the arguments */

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

    signal(SIGINT,  handle_sigint_parent);
    signal(SIGQUIT, handle_sigint_parent);
    signal(SIGCHLD, handle_SIGCHLD_parent);
    
    paths = new string[2 * numMonitors];
    monitorInfo  = new Info[numMonitors];              /* Info array with read file descriptor , write fd and monitor id */

    /* Open directory */
    DIR *inputDIR = opendir(inputDir.c_str());
    if (ENOENT == errno) {
        perror("Couldn't open input directory");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2 * numMonitors; i++){
    
        paths[i] = "/tmp/myFifo." + to_string(i);

        if (mkfifo(paths[i].c_str(), 0666) == -1){
            if (errno != EEXIST){
                perror("ERROR\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    int pid;
    /* fork() numMonitors child processes */
    for (int i = 0; i < 2 * numMonitors; i+=2){ 

        if ((pid = fork()) == 0){
            
            /* paths[x] = writeFd && paths[x + 1] = readFd */
            const char* args[] = {"./Monitor", paths[i].c_str(), paths[i + 1].c_str(), NULL};

            if (execvp("./Monitor", (char* const*) args) == -1){
                cout << "Failed execvp!" << endl;
                return 0;
            }
            exit(0);

        }else{
            monitorInfo[i/2].monitorId = pid;
        }
    }

    int writefd;
    int readfd;

    int currentCountry = 0;
    for (int i = 0; i < 2 * numMonitors; i+=2){
        
        if ((writefd = open(paths[i].c_str(), 1))  < 0){  
            perror("server: can't open write fifo");
            exit(EXIT_FAILURE);
        }

        if ((readfd = open(paths[i + 1].c_str(), 0))  < 0){  
            perror("server: can't open write fifo");
            exit(EXIT_FAILURE);
        }

        /* Monitor Info*/
        monitorInfo[i/2].writeFd = writefd;
        monitorInfo[i/2].readFd = readfd;

        writeInt(socketBufferSize,writefd);
        writeInt(sizeOfBloom,writefd);     
        writeString(inputDir.c_str(),socketBufferSize,writefd);
    }     

    struct dirent *counter;
    int countCountries = 0;

    while ((counter = readdir(inputDIR)) != NULL){      /* Read the file and count the countries */
        
        if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) 
            continue;
    
        string directory = counter->d_name;
        const char * directoryToSend = directory.c_str();

        writeString(directoryToSend, socketBufferSize, monitorInfo[countCountries].writeFd);        /* Send directory (countryName) to monitor */

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

    int endFlag = -1;
    for (int i = 0; i < numMonitors; i++){              /* Read in child (travelMonitor) until endFlag = -1 */
        writeInt(endFlag, monitorInfo[i].writeFd);
    }

    /* Read the bloom filters */
    fd_set readFdsSet;
    string messageRead;

    FD_ZERO(&readFdsSet);
    for(int i = 0; i < numMonitors; i++){
        FD_SET(monitorInfo[i].readFd, &readFdsSet);
    }
    
    int maxfd = monitorInfo[0].readFd;
    for (int i = 1; i < numMonitors; i++){
        if (monitorInfo[i].readFd > maxfd){
            maxfd = monitorInfo[i].readFd;
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
            
            FD_SET(monitorInfo[i].readFd, &readFdsSet);

            if (FD_ISSET(monitorInfo[i].readFd, &readFdsSet)){

                /* Get the number of viruses (equals with the number of bloom filters) */
                int numOfViruses = readInt(monitorInfo[i].readFd);

                for (int j = 0; j < numOfViruses; j++){                                         /* How many bloom filters to read */

                    string virusNameStr = readString(monitorInfo[i].readFd, socketBufferSize);        /* Get the virus name */
                    tempBloom = readBloom(monitorInfo[i].readFd, socketBufferSize,sizeOfBloom);

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
    
    for (int i = 0; i < numMonitors; i++){
        kill(monitorInfo[i].monitorId, SIGINT);
        pause();                                        // wait for signal
    }
    
    /* ------------------------------------------ User's commands ------------------------------------------ */

    do{   /* Display the menu with the user commands */

    }while (!Commands(sizeOfBloom, numMonitors, socketBufferSize, monitorInfo, bloomListHead, &req));

    kill(getpid(), SIGINT);
    cout << "Program completed successfully." << endl;
    return 0;
}

void handle_sigint_parent(int sig){

    suicide = true;

    /* Send SIGKILL signal to each monitor => command "/exit" was used */
    for (int i = 0; i < numMonitors; i++)
        kill(monitorInfo[i].monitorId, SIGKILL);
    
    /* Wait for each child to finish */
    for (int i = 0; i < numMonitors; i++)
        waitpid(monitorInfo[i].monitorId, NULL, 0);
    
    /* Unlink the named pipes */
    for (int i = 0; i < 2 * numMonitors; i++)
        unlink(paths[i].c_str());

    // string logFileStr = "log_file." + to_string(getpid()) + ".txt";
    // ofstream logFile;
    // logFile.open(logFileStr);                                           /* Create and open the file */

    // CountryListPrintInFile(countryListHead,logFile);                    /* Print all the countries in the logFile */
    // logFile << "TOTAL TRAVEL REQUESTS " << req.totalReq << endl;        /* Print total travel requests in the logfile */
    // logFile << "ACCEPTED " << req.acceptedReq << endl;                  /* Print accepted requests in the logfile */
    // logFile << "REJECTED " << req.rejectedReq << endl;                  /* Print rejected requests in the logfile */

    // logFile.close();                                                    /* Close the file */
}

void handle_SIGCHLD_parent(int sig){

    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) <= 0){
    }

    if (suicide)        /* If travelMonitor commits suicide ignore SIGCHLD signal => don't fork monitors again '*/
        return;

    int tempIndex;

    for (int i = 0; i < numMonitors; i++){
        if (monitorInfo[i].monitorId == pid){
            tempIndex = i;
        }
    }
        
    if ((pid = fork()) == 0){

        /* paths[x] = writeFd && paths[x + 1] = readFd */
        const char* args[] = {"./Monitor", paths[tempIndex*2].c_str(), paths[(tempIndex*2) + 1].c_str(), (char*) NULL};

        if (execvp(args[0], (char* const*) args) == -1){
            cout << "Failed execvp!" << endl;
            return;
        }
        exit(0);

    }else{
        monitorInfo[tempIndex].monitorId = pid;
    }
        
    if ((monitorInfo[tempIndex].writeFd = open(paths[2*tempIndex].c_str(), 1))  < 0){  
        perror("server: can't open write fifo");
        exit(EXIT_FAILURE);
    }

    if ((monitorInfo[tempIndex].readFd = open(paths[(2*tempIndex) + 1].c_str(), 0))  < 0){  
        perror("server: can't open write fifo");
        exit(EXIT_FAILURE);
    }

    writeInt(socketBufferSize,monitorInfo[tempIndex].writeFd);
    writeInt(sizeOfBloom,monitorInfo[tempIndex].writeFd);     
    writeString(inputDir.c_str(),socketBufferSize,monitorInfo[tempIndex].writeFd);

    /* Open directory */
    DIR *inputDIR = opendir(inputDir.c_str());
    
    if (ENOENT == errno) {
        perror("Couldn't open input directory");
        exit(EXIT_FAILURE);
    }
  
    CountryNode* tempCountryNode = monitorInfo[tempIndex].countryListHead;

    /* Send the counties to the monitor with index "tempIndex" */
    while (tempCountryNode != NULL){
        writeString((tempCountryNode->country).c_str(),socketBufferSize,monitorInfo[tempIndex].writeFd);
        tempCountryNode = tempCountryNode->next;
    }

    int endFlag = -1;
    writeInt(endFlag, monitorInfo[tempIndex].writeFd);          /* Read in child (travelMonitor) until endFlag = -1 */

    /* Read the bloom filters */
    char* tempBloom;

    /* Get the number of viruses (equals with the number of bloom filters) */
    int numOfViruses = readInt(monitorInfo[tempIndex].readFd);

    for (int j = 0; j < numOfViruses; j++){             /* How many bloom filters to read */

        string virusNameStr = readString(monitorInfo[tempIndex].readFd, socketBufferSize);                    /* Get the virus name */
        tempBloom = readBloom(monitorInfo[tempIndex].readFd, socketBufferSize,sizeOfBloom);

        /* Create the list with the bloom filters */
        if (!(BloomListSearch(bloomListHead, virusNameStr)))                        /* If virusName is not already in the list => add it (in the beginning) */
            BloomListPush(&bloomListHead, virusNameStr, tempBloom, sizeOfBloom);
        else
            BloomListMergeBlooms(bloomListHead, virusNameStr, tempBloom);

        delete tempBloom;
    }
}