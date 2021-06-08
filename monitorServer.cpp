#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "ReadFile.h"
#include "SocketFunctions.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fstream>

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <pthread.h>

using namespace std;

struct CountryNode* countryListHead = NULL;     /* Create and initialize country list head */
struct CitizenNode* citizenListHead = NULL;     /* Create and initialize citizen list head */
struct VirusNode* virusListHead = NULL;         /* Create and initialize virus list head */

int sizeOfBloom;
int numThreads;
int cyclicBufferSize;

typedef struct {
    string* data;
    int start;
    int end;
    int count;
} pool_t;

int num_of_items = 0;
string* countries; 
string* txts;

pthread_mutex_t mtx;
pthread_mutex_t mtxRead;
pthread_cond_t cond_nonempty;
pthread_cond_t cond_readFile;
pthread_cond_t cond_nonfull;
pool_t pool;

void initialize(pool_t * pool, int cyclicBufferSize) {
    pool->data = new string[cyclicBufferSize];
    pool->start = 0;
    pool->end = -1;
    pool->count = 0;
}

void place(pool_t * pool, string data) {
    pthread_mutex_lock(&mtx);

    while (pool->count >= cyclicBufferSize) {
        printf(">> Found Buffer Full \n");
        pthread_cond_wait(&cond_nonfull, &mtx);
    }

    pool->end = (pool->end + 1) % cyclicBufferSize;
    pool->data[pool->end] = data;
    pool->count++;
    pthread_mutex_unlock(&mtx);
}

string obtain(pool_t * pool) {
    string data = "";
    pthread_mutex_lock(&mtx);

    while (pool->count <= 0) {
        printf(">> Found Buffer Empty \n");
        pthread_cond_wait(&cond_nonempty, &mtx);
    }
    data = pool->data[pool->start];
    pool->start = (pool->start + 1) % cyclicBufferSize;
    pool->count--;
    pthread_mutex_unlock(&mtx);
    return data;
}

void * producer(void * ptr){
    int index = 0;
    while (num_of_items > 0) {
        place(&pool, txts[index++]);
        num_of_items--;
        pthread_cond_signal(&cond_nonempty);
    }
    cout << "eftase sto exit producer" << endl;
    return NULL;
}

void * consumer(void * ptr){
    while (num_of_items > 0 || pool.count > 0) {

        string path = obtain(&pool);
        cout << "consumer: " << path << endl;
       
        readFile(path, sizeOfBloom, &citizenListHead, &virusListHead, &countryListHead);
        // usleep(500);
        pthread_cond_signal(&cond_nonfull);
        //usleep(500000);
    }
    cout << "to paidi kanei exit " << endl;
    pthread_exit(0);
}

void* babasThread(void * ptr){

    pthread_t *paidia;

    initialize(&pool,cyclicBufferSize);
    pthread_mutex_init(&mtx, 0);
    pthread_mutex_init(&mtxRead, 0);
    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);
    pthread_cond_init(&cond_readFile, 0);

    paidia = new pthread_t[numThreads];

    for (int i = 0; i < numThreads; i++){
        pthread_create(&paidia[i], 0, consumer, 0);
    }

    producer(NULL);
    
    for (int i = 0; i < numThreads; i++){
       if (pthread_join(paidia[i],0) != 0){
           perror("paidi tou baba\n");
           exit(1);
       }
    }
    cout << "TELEIWNEI O BABAS STO THREAD \n";
    pthread_exit(0);
}


int main(int argc, char *argv[]){

    if (argc != 12){                                 /* There must be at least 1 path */
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    int port, socketBufferSize;

    for (int i = 1; i < argc - 1; i+=2){           /* Get all the arguments */

        if (strcmp(argv[i],"-p") == 0)              /* Get the port number */
            port = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-b") == 0)               /* Get the size of the socketBuffer */
            socketBufferSize = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-c") == 0)                   /* Get the size of the CyclicBuffer */
            cyclicBufferSize = stoi(argv[i+1]);

        else if (strcmp(argv[i],"-s") == 0)                   /* Get the size of the bloom */
            sizeOfBloom = stoi(argv[i+1]);
        
        else if (strcmp(argv[i],"-t") == 0)                   /* Get the number of Threads */
            numThreads = stoi(argv[i+1]);
    
        else{
           perror("Error getting the arguments");
           exit(EXIT_FAILURE);
        } 
    }
    string allCountries(argv[11]);  

    cout << "Oi xwres 2 einai: " << allCountries << endl;
    cout << "ELABA TO PORT: " << port << endl;

    /* ------------------------------ SOCKETS ------------------------------ */

    int newSocket;
    int server_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
       
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
       
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((newSocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    /* ------------------------------ END OF SOCKETS ------------------------------ */

    string inputDir = readString(newSocket,socketBufferSize);            /* Get the name of the inputDir from travelMonitor */

    cout << "to sizeOfBloom einai: " << sizeOfBloom << endl;
    cout << "to buffer size einai: " << socketBufferSize << endl;
    cout << "to input dir onoma einai: " << inputDir << endl;

    stringstream ss(allCountries);
    stringstream ss2(allCountries);
    string country;

    int numOfCountries = 0;

    /* count the countries */
    while (ss2 >> country)
        numOfCountries++;
    
    countries = new string[numOfCountries];    

    int i = 0;
    while (ss >> country) 
        countries[i++] = inputDir + "/" + country;

    cout << "numOfCountries: " << numOfCountries << endl;
       
    struct dirent *counter;
    DIR *countryDir;

    /* -------------------------------- threads -------------------------------- */

    pthread_t cons;
    pthread_t babasThr;

    for (int i = 0; i < numOfCountries; i++){           /* For each country (or each file) */

        /* Open country directory */
        countryDir = opendir(countries[i].c_str());
        if (ENOENT == errno){
            perror("Couldn't open country directory");
            exit(EXIT_FAILURE);
        }
    
        while ((counter = readdir(countryDir)) != NULL){       /* For each .txt file inside CountryFileName */
        
            if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) 
                continue;

            num_of_items++;
        }
        rewinddir(countryDir);
    }

    txts = new string[num_of_items];
    int counter2 = 0;

    for (int i = 0; i < numOfCountries; i++){            /* For each country (or each file) */

        /* Open country directory */
        countryDir = opendir(countries[i].c_str());
        if (ENOENT == errno){
            perror("Couldn't open country directory");
            exit(EXIT_FAILURE);
        }
    
        while ((counter = readdir(countryDir)) != NULL){       /* For each .txt file inside CountryFileName */
        
            if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) 
                continue;

            txts[counter2++] = countries[i] + "/" + counter->d_name;
        }
        rewinddir(countryDir);
    }

    cout << "o arithmos twn txt: " << num_of_items << endl;
    pthread_create(&babasThr, 0, babasThread, 0);
    pthread_join(babasThr, 0);
    cout << "TELEIWSW O BABAS\n";

    /* ---------------------------------------------------------------- */

    int numOfViruses = VirusListCount(virusListHead);            /* Get the number of viruses (equals with the number of bloom filters) */
    sendInt(newSocket, numOfViruses);                         /* Send the number of viruses (equals with the number of bloom filters) */

    /* Send back to parent proccess (travelMonitor) numofViruses bloom filters (one bloom filter per virus) */
    for (int i = 0; i < numOfViruses; i++){

        string virusName = VirusListGetVirusName(virusListHead,i);
        sendString(newSocket,virusName.c_str(), socketBufferSize);                                                /* Send the virus name */
        sendBloom(newSocket, VirusGetBloomArray(virusListHead,virusName), sizeOfBloom, socketBufferSize);            /* Send the bloom filter */
    }
    
    int acceptedReq = 0;
    int rejectedReq = 0;
    int totalReq = 0;

    while(1){       /* While the user is giving commands */

        string command = readString(newSocket,socketBufferSize);             /* Get command's name e.g. "/travelRequest" or "/searchVaccinationStatus" */

        if (command.compare("/searchVaccinationStatus") == 0){      /* Command 4 - /searchVaccinationStatus citizenID */

            int citizenId = readInt(newSocket);                            /* Get the citizen's Id from travelMonitor */
            struct CitizenNode *tempCit;
            int foundFlag = -1;

            if (CitizenListSearch(citizenListHead,citizenId,&tempCit))      /* Check if citizen Id is valid (exists in CitizenList) */
                foundFlag = 1;
            
            sendInt(newSocket,foundFlag);
            
            if (foundFlag == 1){

                int age = tempCit->citizen.age;
                string firstName = tempCit->citizen.firstName;
                string lastName = tempCit->citizen.lastName;
                string country = *(tempCit->citizen.country);

                sendString(newSocket, firstName.c_str(), socketBufferSize);
                sendString(newSocket, lastName.c_str(), socketBufferSize);
                sendString(newSocket, country.c_str(), socketBufferSize);
                sendInt(newSocket,age);

                VirusNode* current = virusListHead;
                VInfoNode* found;
                CitizenNode* cit;

                while (current != NULL){                                            /* For each virus => Search the vaccinated skip list */
                    if (current->Vaccinated->SkipListSearch(citizenId,&cit)){        /* If person is vaccinated => send vaccination info */
                        
                        sendString(newSocket, "YES", socketBufferSize);
                        found = VInfoListSearch2(cit->citizen.citizenIDptr,current->virusName);
                
                        string dateStr = found->vacInfo.dateVaccinated->convertDateToStr();
                        
                        sendString(newSocket,(current->virusName).c_str(),socketBufferSize);
                        sendString(newSocket,dateStr.c_str(),socketBufferSize);

                    }else{                                                      /* Not vaccinated => there is no date */
                        sendString(newSocket, "NO", socketBufferSize);
                        sendString(newSocket,(current->virusName).c_str(),socketBufferSize);
                    }
                    current = current->next;        /* Go to the next virus */
                }
                sendString(newSocket, "STOPPED", socketBufferSize);
            }

        }else if (command.compare("/travelRequest") == 0){          /* Command 1 - /travelRequest citizenID date countryFrom countryTo virusName */

            int citizenId = readInt(newSocket);                            /* Get the citizen's Id from travelMonitor */
            struct CitizenNode *tempCit;
            string virusName = readString(newSocket, socketBufferSize);

            Date* date = VirusListCheckIdForVirus(virusListHead,citizenId,virusName);
            totalReq++; 

            if (date == NULL){
                sendString(newSocket,"NO",socketBufferSize);
                rejectedReq++;
            }else{
                sendString(newSocket,"YES",socketBufferSize);
                acceptedReq++;

                /* convert date to string */
                string dateStr = date->convertDateToStr();

                sendString(newSocket,dateStr.c_str(), socketBufferSize);
            }

        }else if (command.compare("/addVaccinationRecords") == 0){

            string country = inputDir + "/" + readString(newSocket, socketBufferSize);
            struct dirent *counter;
            DIR *countryDir;

            /* Open country directory */
            countryDir = opendir(country.c_str());

            if (ENOENT == errno){
                perror("Couldn't open country directory");
                exit(EXIT_FAILURE);
            }

            while ((counter = readdir(countryDir)) != NULL){                     /* For each .txt file inside CountryFileName */
            
                if ((!strcmp(counter->d_name, ".") || !strcmp(counter->d_name, ".."))) 
                    continue;

                readFile(country + "/" + counter->d_name, sizeOfBloom, &citizenListHead, &virusListHead, &countryListHead);
            }
            rewinddir(countryDir);

            int numOfViruses = VirusListCount(virusListHead);

            /* Send the number of viruses (equals with the number of bloom filters) */
            sendInt(newSocket, numOfViruses);

            /* Send back to parent proccess (travelMonitor) numofViruses bloom filters (one bloom filter per virus) */
            for (int i = 0; i < numOfViruses; i++){

                string virusName = VirusListGetVirusName(virusListHead,i);
                sendString(newSocket, virusName.c_str(), socketBufferSize);                                         /* Send the virus name */
                sendBloom(newSocket, VirusGetBloomArray(virusListHead,virusName),sizeOfBloom,socketBufferSize);       /* Send the bloomFilter */
            }

            cout << "New vaccination records added successfully." << endl;

        }else if (command.compare("/exit") == 0){

            // string logFileStr = "log_file." + to_string(getpid()) + ".txt";
            // ofstream logFile;
            // logFile.open(logFileStr);                                   /* Create and open the file */

            // CountryListPrintInFile(countryListHead,logFile);            /* Print all the countries in the logFile */
            // logFile << "TOTAL TRAVEL REQUESTS " << totalReq << endl;    /* Print total travel requests in the logfile */
            // logFile << "ACCEPTED " << acceptedReq << endl;              /* Print accepted requests in the logfile */
            // logFile << "REJECTED " << rejectedReq << endl;              /* Print rejected requests in the logfile */

            // logFile.close();

            close(newSocket);

            /* Delete the lists */
            VirusDeleteList(&virusListHead);
            CountryDeleteList(&countryListHead);
            CitizenDeleteList(&citizenListHead);
            exit(0);
        }
    }
    return 0;
}