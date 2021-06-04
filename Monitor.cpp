#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "ReadFile.h"
#include "SocketFunctions.h"

#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fstream>

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h> /* gethostbyaddr */

using namespace std;

int newSocket;

struct CountryNode* countryListHead = NULL;     /* Create and initialize country list head */
struct CitizenNode* citizenListHead = NULL;     /* Create and initialize citizen list head */
struct VirusNode* virusListHead = NULL;         /* Create and initialize virus list head */

int acceptedReq = 0;
int rejectedReq = 0;
int totalReq = 0;

/* Read from travelMonitor */
// int socketBufferSize;
// int sizeOfBloom;
// string inputDir;

void handler_SIGINT_and_SIGQUIT(int sig);

int main(int argc, char *argv[]){

    if (argc < 12){                                 /* There must be at least 1 path */
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT,  handler_SIGINT_and_SIGQUIT);                      /* catch SIGINT signal   */
    signal(SIGQUIT, handler_SIGINT_and_SIGQUIT);                      /* catch SIGQUIT signal   */

    int port = atoi(argv[2]);
    int numThreads = atoi(argv[4]);
    int socketBufferSize  = atoi(argv[6]);   
    int cyclicBufferSize  = atoi(argv[8]);                                      
    int sizeOfBloom = atoi(argv[10]);                      

    cout << "ELABA TO PORT: " << port << endl;

    /* ------------------------------ SOCKETS ------------------------------ */

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

    string allCountries = "";       /* A string that will contain all countries seperated by " " */                               
    int numOfCountries = 0;         /* The total number of countries */
    do{
        
        int sizeOfCountryFile = readInt(newSocket);
                
        if (sizeOfCountryFile == -1)                   /* endFlag located => stop reading */
            break;

        char countryFileName[sizeOfCountryFile];
        int counter = 0;

        for (int i = 0; i < sizeOfCountryFile/socketBufferSize; i++){
            
            if (read(newSocket, &(countryFileName[counter]), socketBufferSize) == -1){
                perror("Couldnt write strSize");
                exit(EXIT_FAILURE);
            }
            counter += socketBufferSize;
        }

        if (counter < sizeOfCountryFile){     /* write the remaining bytes */
                    
            int tempBuff = sizeOfCountryFile - counter;       /* tempBuff will always be smaller that the socketBufferSize */

            if (read(newSocket, &(countryFileName[counter]), tempBuff) == -1){
                perror("Couldnt write strSize");
                exit(EXIT_FAILURE);
            }
        }
        countryFileName[sizeOfCountryFile] = '\0';

        numOfCountries++;
        string temp(countryFileName);
        allCountries = allCountries + " " + temp;

    }while(1);

    stringstream ss(allCountries);
    string country;
    string countries[numOfCountries];       

    int i = 0;
    while (ss >> country) 
        countries[i++] = inputDir + "/" + country;
       
    struct dirent *counter;
    DIR *countryDir;

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

            readFile(countries[i] + "/" + counter->d_name, sizeOfBloom, &citizenListHead, &virusListHead, &countryListHead);
        }
        rewinddir(countryDir);
    }

    int numOfViruses = VirusListCount(virusListHead);            /* Get the number of viruses (equals with the number of bloom filters) */
    sendInt(newSocket, numOfViruses);                         /* Send the number of viruses (equals with the number of bloom filters) */

    /* Send back to parent proccess (travelMonitor) numofViruses bloom filters (one bloom filter per virus) */
    for (int i = 0; i < numOfViruses; i++){

        string virusName = VirusListGetVirusName(virusListHead,i);
        sendString(newSocket,virusName.c_str(), socketBufferSize);                                                /* Send the virus name */
        sendBloom(newSocket, VirusGetBloomArray(virusListHead,virusName), sizeOfBloom, socketBufferSize);            /* Send the bloom filter */
    }

    while(1){       /* While the user is giving commands */

        string command = readString(newSocket,socketBufferSize);             /* Get command's name e.g. "/travelRequest" or "/searchVaccinationStatus" */

        if (command.compare("/searchVaccinationStatus") == 0){      /* Command 4 - /searchVaccinationStatus citizenID */

            int citizenId = readInt(newSocket);                            /* Get the citizen's Id from travelMonitor */
            struct CitizenNode *tempCit;
            int foundFlag = -1;

            if (CitizenListSearch(citizenListHead,citizenId,&tempCit))      /* Check if citizen Id is valis (exists in CitizenList) */
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
        }
    }
    return 0;
}


/* ---------------------------------------------------------------- */


void handler_SIGINT_and_SIGQUIT(int sig){

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