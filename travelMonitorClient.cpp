#include <iostream>
#include <sstream>

#include "BloomFilter.h"
#include "CountryList.h"
#include "bloomList.h"
#include "Commands.h"

#include <fstream>
#include <dirent.h>
#include <fcntl.h>

#define PORT 9020

using namespace std;

int main(int argc, char *argv[]){

    if (argc != 13){
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    int numMonitors, socketBufferSize, cyclicBufferSize, sizeOfBloom, numThreads;
    string inputDir;
    
    for (int i = 1; i < argc; i+=2){           /* Get all the arguments */

        if (strcmp(argv[i],"-m") == 0)           /* Get the number of monitors */
            numMonitors = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-b") == 0)         /* Get the size of the socketBuffer */
            socketBufferSize = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-c") == 0)         /* Get the size of the CyclicBuffer */
            cyclicBufferSize = stoi(argv[i+1]);

        else if (strcmp(argv[i],"-s") == 0)         /* Get the size of the bloom */
            sizeOfBloom = stoi(argv[i+1]);

        else if (strcmp(argv[i],"-i") == 0)          /* Get the input directory */
            inputDir = argv[i+1];
        
        else if (strcmp(argv[i],"-t") == 0)         /* Get the number of Threads */
            numThreads = stoi(argv[i+1]);
    
        else{
           perror("Error getting the arguments");
           exit(EXIT_FAILURE);
        } 
    }
 
    struct BloomNode* bloomListHead = NULL;              /* List with a bloom filter for each virus */
    struct CountryNode* countryListHead = NULL;          /* One list with all the countries */
    
    struct dirent **countryFiles;
    int numOfCountries = 0;
    int countCountries = 0;

    numOfCountries = scandir(inputDir.c_str(), &countryFiles, 0, alphasort);
    numOfCountries = numOfCountries - 2;                                            /* ignore the current "." and the previous directory ".." */

    if (numOfCountries < 0){                        /* error occurred with scandir */
        perror("Couldn't open input directory");
        exit(EXIT_FAILURE);
    }
    
    /* if countries are less than the number of monitors => then only fork numOfCountries monitorServers (one country per monitorServer) */
    if (numMonitors > numOfCountries){
        numMonitors = numOfCountries;
    }

    Info monitorInfo[numMonitors];              /* Info array with socket fd, proccess id and a list of countries */

    
    for (int i = 0; i < numOfCountries; i++) {       /* for each country */

        if ((!strcmp(countryFiles[i]->d_name, ".") || !strcmp(countryFiles[i]->d_name, ".."))) 
            continue;

        string directory = countryFiles[i]->d_name;

        /* Create the list with the countries */
        if (!(CountryListSearch(countryListHead, directory))){     /* If country is not already in the list => add it (in the beginning) */
            CountryListPush(&countryListHead, directory);
        }

        CountryListPush(&(monitorInfo[countCountries].countryListHead), directory);
        countCountries++;

        if (countCountries == numMonitors)
            countCountries = 0;
        
        delete(countryFiles[i]);
    }
        
    delete(countryFiles);

    int pid;
    for (int i = 0; i < 2 * numMonitors; i+=2){      /* fork() numMonitors child processes */

        if ((pid = fork()) == 0){
            
            string allCountries = getAllCountries(monitorInfo[i/2].countryListHead, inputDir);

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
        string hostName;
        
        struct hostent* host;

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            cout << "Socket creation error " << endl;
            exit(EXIT_FAILURE);
        }
        
        gethostname(&hostName[0], 1023);
        host = gethostbyname(&hostName[0]);
    
        memset(&serverAddr, '\0', sizeof(serverAddr));
        memcpy(&serverAddr.sin_addr, host->h_addr, host->h_length);

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons((PORT+i));

        /* wait until the socket connects on the other side (monitorServer) */
        do
            connectStatus = connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        while (connectStatus < 0);

        monitorInfo[i].socketFd = sock;
    }

    /* ------------------------------ END OF SOCKETS ------------------------------ */

    /* Read the bloom filters */
    fd_set readFdsSet;

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

    /* Get all the bloom filters from the monitorServers (using select) */
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
        
    NumOfRequests req;      /* struct to count the requests */

    /* ------------------------------------------ User's commands ------------------------------------------ */

    do{   /* Display the menu with the user commands */

    }while (!Commands(sizeOfBloom, numMonitors, socketBufferSize, monitorInfo, bloomListHead, &req, inputDir));

    /* Wait for each child to finish */
    for (int i = 0; i < numMonitors; i++)
        waitpid(monitorInfo[i].monitorId, NULL, 0);
    
    /* close the socket connections */
    for (int i = 0; i < numMonitors; i++){
        close(monitorInfo[i].socketFd);
    }

    /* Create the log_file txt */
    string logFileStr = "log_file." + to_string(getpid()) + ".txt";
    ofstream logFile;
    logFile.open(logFileStr);                                           /* Create and open the file */

    CountryListPrintInFile(countryListHead,logFile);                    /* Print all the countries in the logFile */
    logFile << "TOTAL TRAVEL REQUESTS " << req.totalReq << endl;        /* Print total travel requests in the logfile */
    logFile << "ACCEPTED " << req.acceptedReq << endl;                  /* Print accepted requests in the logfile */
    logFile << "REJECTED " << req.rejectedReq << endl;                  /* Print rejected requests in the logfile */

    logFile.close();                                                    /* Close the file */

    CountryDeleteList(&countryListHead);            /* Delete the country list */
    cout << "Program completed successfully." << endl;
    return 0;
}
