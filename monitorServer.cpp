#include <iostream>

#include "ReadFile.h"
#include "SocketFunctions.h"

#include <dirent.h>
#include <fcntl.h>
#include <fstream>

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

int numOfFiles = 0;         /* The total number of .txt files */

string* txtFilePaths;
string* countryTxtFiles;

pthread_mutex_t mutex;
pthread_mutex_t mutexRead;

pthread_cond_t cond_nonempty;
pthread_cond_t cond_nonfull;

pool_t pool;

void initialize(pool_t * pool, int cyclicBufferSize);
void place(pool_t * pool, string data);
string obtain(pool_t * pool);
void * producer(void * ptr);                                /* Used for the command "/addVaccinationRecords" */
void * producerForOneCountry(void * ptr);
void * consumer(void * ptr);

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

    /* ------------------------------ SOCKETS ------------------------------ */

    int sock, newSocket;
    int option = 1;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
       
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("error in socket creation: ");
        exit(EXIT_FAILURE);
    }

    /* So the address can be use again immediately */
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option))){
        perror("error in setsockopt: ");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
       
    if (bind(sock, (struct sockaddr *)&address, sizeof(address)) < 0){
        perror("error in bind: ");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) < 0){
        perror("error in listen: ");
        exit(EXIT_FAILURE);
    }

    if ((newSocket = accept(sock, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("error in accept: ");
        exit(EXIT_FAILURE);
    }

    /* ------------------------------ END OF SOCKETS ------------------------------ */

    stringstream ss(allCountries);
    stringstream ss2(allCountries);
    string country;

    int numOfCountries = 0;

    /* count the countryPaths */
    while (ss2 >> country)
        numOfCountries++;
    
    string countryPaths[numOfCountries];

    int i = 0;
    while (ss >> country)                               /* create the paths */
        countryPaths[i++] = country;
       
    struct dirent ** txtFilesList;
    
    /* Count the all the record files (txts) */
    for (int i = 0; i < numOfCountries; i++){    /* For every country file */

        int currNumOfFiles = scandir(countryPaths[i].c_str(), &txtFilesList, 0, alphasort);

        if (currNumOfFiles < 0){                /* error occurred with scandir */
            perror("Couldn't open directory");
            exit(EXIT_FAILURE);
        }
        numOfFiles += currNumOfFiles - 2;     /* ignore the current "." and the previous directory ".." */
    }

    txtFilePaths = new string[numOfFiles];
    int index = 0;

    for (int i = 0; i < numOfCountries; i++){       /* For every country file */

        int currNumOfFiles = scandir(countryPaths[i].c_str(), &txtFilesList, 0, alphasort);

        if (currNumOfFiles < 0){                        /* error occurred with scandir */
            perror("Couldn't open directory");
            exit(EXIT_FAILURE);
        }
        
        for (int j = 0; j < currNumOfFiles; j++){           /* For every .txt file inside country file => add it in the array with the txtFiles */

            if ((!strcmp(txtFilesList[j]->d_name, ".") || !strcmp(txtFilesList[j]->d_name, ".."))) 
                continue;

            txtFilePaths[index++] = countryPaths[i] + "/" + txtFilesList[j]->d_name;
            delete(txtFilesList[j]);
        }
    }

    /* -------------------------------- threads -------------------------------- */

    pthread_t consumers[numThreads];       /* array with the threads */

    initialize(&pool,cyclicBufferSize);
    pthread_mutex_init(&mutex, 0);
    pthread_mutex_init(&mutexRead, 0);

    pthread_cond_init(&cond_nonempty, 0);
    pthread_cond_init(&cond_nonfull, 0);

    for (int i = 0; i < numThreads; i++){
        pthread_create(&consumers[i], 0, consumer, 0);
    }

    producer(NULL);
    
    for (int i = 0; i < numThreads; i++){
       if (pthread_join(consumers[i], NULL) != 0){
           perror("error waiting\n");
           exit(EXIT_FAILURE);
       }
    }
    
    /* -------------------------------- end of threads --------------------------------- */

    int numOfViruses = VirusListCount(virusListHead);         /* Get the number of viruses (equals with the number of bloom filters) */
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

                    }else if (current->NotVaccinated->SkipListSearch(citizenId,&cit)){                                                      /* Not vaccinated => there is no date */
                        sendString(newSocket, "NO", socketBufferSize);
                        sendString(newSocket,(current->virusName).c_str(),socketBufferSize);
                    }else{
                        sendString(newSocket, "CONTINUE", socketBufferSize);
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

            string inputDir = readString(newSocket,socketBufferSize);            /* Get the name of the inputDir from travelMonitor */
            string country = inputDir + "/" + readString(newSocket, socketBufferSize);

            struct dirent *counter;
            DIR *countryDir;

            struct dirent ** txtFilesList2;
            int numOfFiles2 = scandir(country.c_str(), &txtFilesList2, 0, alphasort);
            
            countryTxtFiles = new string[numOfFiles2];

            if (numOfFiles2 < 0){                        /* error occurred with scandir */
                perror("Couldn't open directory");
                exit(EXIT_FAILURE);
            }

            int countTxts = 0;
            for (int j = 0; j < numOfFiles2; j++){           /* For every .txt file inside country file */

                if ((!strcmp(txtFilesList2[j]->d_name, ".") || !strcmp(txtFilesList2[j]->d_name, ".."))) 
                    continue;

                countryTxtFiles[countTxts++] = country + "/" + txtFilesList2[j]->d_name;
                delete(txtFilesList2[j]);
            }
            delete(txtFilesList2);

            numOfFiles = countTxts;

            pthread_t consumers[numThreads];       /* array with the threads */

            initialize(&pool,cyclicBufferSize);
            pthread_mutex_init(&mutex, 0);
            pthread_mutex_init(&mutexRead, 0);

            pthread_cond_init(&cond_nonempty, 0);
            pthread_cond_init(&cond_nonfull, 0);

            for (int i = 0; i < numThreads; i++){
                pthread_create(&consumers[i], 0, consumer, 0);
            }

            producerForOneCountry(NULL);
            
            for (int i = 0; i < numThreads; i++){
                if (pthread_join(consumers[i], NULL) != 0){
                    perror("error waiting\n");
                    exit(EXIT_FAILURE);
                }
            }

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

            string logFileStr = "log_file." + to_string(getpid()) + ".txt";
            ofstream logFile;
            logFile.open(logFileStr);                                   /* Create and open the file */

            CountryListPrintInFile(countryListHead,logFile);            /* Print all the countryPaths in the logFile */
            logFile << "TOTAL TRAVEL REQUESTS " << totalReq << endl;    /* Print total travel requests in the logfile */
            logFile << "ACCEPTED " << acceptedReq << endl;              /* Print accepted requests in the logfile */
            logFile << "REJECTED " << rejectedReq << endl;              /* Print rejected requests in the logfile */

            logFile.close();

            close(newSocket);
            close(sock);

            /* Delete the lists */
            VirusDeleteList(&virusListHead);
            CountryDeleteList(&countryListHead);
            CitizenDeleteList(&citizenListHead);
            exit(0);
        }
    }
    return 0;
}

/* ---------------------------------------------------------------- */

void initialize(pool_t * pool, int cyclicBufferSize) {
    pool->data = new string[cyclicBufferSize];
    pool->start = 0;
    pool->end = -1;
    pool->count = 0;
}

void place(pool_t * pool, string data) {
    pthread_mutex_lock(&mutex);

    while (pool->count >= cyclicBufferSize) {
        pthread_cond_wait(&cond_nonfull, &mutex);
    }

    pool->end = (pool->end + 1) % cyclicBufferSize;
    pool->data[pool->end] = data;
    pool->count++;
    pthread_mutex_unlock(&mutex);
}

string obtain(pool_t * pool) {
    string data = "";
    pthread_mutex_lock(&mutex);

    while (pool->count <= 0) {
        pthread_cond_wait(&cond_nonempty, &mutex);
    }
    data = pool->data[pool->start];
    pool->start = (pool->start + 1) % cyclicBufferSize;
    pool->count--;
    pthread_mutex_unlock(&mutex);
    return data;
}

void * producer(void * ptr){
    int index = 0;
    while (numOfFiles > 0) {
        place(&pool, txtFilePaths[index++]);
        numOfFiles--;
        pthread_cond_signal(&cond_nonempty);
    }
    return NULL;
}

void * producerForOneCountry(void * ptr){
    int index = 0;
    while (numOfFiles > 0) {
        place(&pool, countryTxtFiles[index++]);
        numOfFiles--;
        pthread_cond_signal(&cond_nonempty);
    }
    return NULL;
}

void * consumer(void * ptr){
    while (numOfFiles > 0 || pool.count > 0) {

        string path = obtain(&pool);
        
        pthread_mutex_lock(&mutexRead);
        readFile(path, sizeOfBloom, &citizenListHead, &virusListHead, &countryListHead);
        pthread_mutex_unlock(&mutexRead);

        pthread_cond_signal(&cond_nonfull);
    }
    return NULL;
}
