#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include "ReadFile.h"
#include "PipeFunctions.h"

#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fstream>

using namespace std;

struct CountryNode* countryListHead = NULL;     /* Create and initialize country list head */
struct CitizenNode* citizenListHead = NULL;     /* Create and initialize citizen list head */
struct VirusNode* virusListHead = NULL;         /* Create and initialize virus list head */

int acceptedReq = 0;
int rejectedReq = 0;
int totalReq = 0;

int readFd;
int writeFd;

/* Read from travelMonitor */
int bufferSize;
int sizeOfBloom;
string inputDir;

void handler_SIGINT_and_SIGQUIT(int sig);
void signal_SIGUSR1_Handler(int sig);

int main(int argc, char *argv[]){

    if (argc != 3){
        perror("Invalid number of arguments");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, signal_SIGUSR1_Handler);                          /* catch SIGUSR1 signal */
    signal(SIGINT,  handler_SIGINT_and_SIGQUIT);                      /* catch SIGINT signal   */
    signal(SIGQUIT, handler_SIGINT_and_SIGQUIT);                      /* catch SIGQUIT signal   */

    string readFifo = argv[1];
    string writeFile = argv[2];

    if ((readFd = open(readFifo.c_str(), 0))  < 0) {
        perror("Couldn't open' write fifo");
        exit(EXIT_FAILURE);
    }

    if ((writeFd = open(writeFile.c_str(), 1))  < 0) {
        perror("Couldn't open' write fifo");
        exit(EXIT_FAILURE);
    }

    bufferSize = readInt(readFd);                           /* Get the buffer size from travelMonitor */
    sizeOfBloom = readInt(readFd);                          /* Get the size of bloom filter from travelMonitor */
    inputDir = readString(readFd,bufferSize);               /* Get the name of the inputDir from travelMonitor */

    string allCountries = "";       /* A string that will contain all countries seperated by " " */                               
    int numOfCountries = 0;         /* The total number of countries */
    do{
        
        int sizeOfCountryFile = readInt(readFd);
                
        if (sizeOfCountryFile == -1)                   /* endFlag located => stop reading */
            break;

        char countryFileName[sizeOfCountryFile];
        int counter = 0;

        for (int i = 0; i < sizeOfCountryFile/bufferSize; i++){
            
            if (read(readFd, &(countryFileName[counter]), bufferSize) == -1){
                perror("Couldnt write strSize");
                exit(EXIT_FAILURE);
            }
            counter += bufferSize;
        }

        if (counter < sizeOfCountryFile){     /* write the remaining bytes */
                    
            int tempBuff = sizeOfCountryFile - counter;       /* tempBuff will always be smaller that the bufferSize */

            if (read(readFd, &(countryFileName[counter]), tempBuff) == -1){
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

    int numOfViruses = VirusListCount(virusListHead);       /* Get the number of viruses (equals with the number of bloom filters) */
    writeInt(numOfViruses,writeFd);                         /* Send the number of viruses (equals with the number of bloom filters) */

    /* Send back to parent proccess (travelMonitor) numofViruses bloom filters (one bloom filter per virus) */
    for (int i = 0; i < numOfViruses; i++){

        string virusName = VirusListGetVirusName(virusListHead,i);
        writeString(virusName.c_str(), bufferSize, writeFd);                                                /* Send the virus name */
        writeBloom(VirusGetBloomArray(virusListHead,virusName), bufferSize,sizeOfBloom,writeFd);            /* Send the bloom filter */
    }

    while(1){       /* While the user is giving commands */

        string command = readString(readFd,bufferSize);             /* Get command's name e.g. "/travelRequest" or "/searchVaccinationStatus" */
        int citizenId = readInt(readFd);                            /* Get the citizen's Id from travelMonitor */
        struct CitizenNode *tempCit;

        if (command.compare("/searchVaccinationStatus") == 0){      /* Command 4 - /searchVaccinationStatus citizenID */

            int foundFlag = -1;

            if (CitizenListSearch(citizenListHead,citizenId,&tempCit))      /* Check if citizen Id is valis (exists in CitizenList) */
                foundFlag = 1;
            
            writeInt(foundFlag,writeFd);
            
            if (foundFlag == 1){

                int age = tempCit->citizen.age;
                string firstName = tempCit->citizen.firstName;
                string lastName = tempCit->citizen.lastName;
                string country = *(tempCit->citizen.country);

                writeString(firstName.c_str(),bufferSize,writeFd);
                writeString(lastName.c_str(),bufferSize, writeFd);
                writeString(country.c_str(),bufferSize, writeFd);
                writeInt(age,writeFd);

                VirusNode* current = virusListHead;
                VInfoNode* found;
                CitizenNode* cit;

                while (current != NULL){                                            /* For each virus => Search the vaccinated skip list */
                    if (current->Vaccinated->SkipListSearch(citizenId,&cit)){        /* If person is vaccinated => send vaccination info */
                        
                        writeString("YES",bufferSize,writeFd);
                        found = VInfoListSearch2(cit->citizen.citizenIDptr,current->virusName);
                
                        string dateStr = found->vacInfo.dateVaccinated->convertDateToStr();
                        
                        writeString((current->virusName).c_str(),bufferSize,writeFd);
                        writeString(dateStr.c_str(),bufferSize,writeFd);

                    }else{                                                      /* Not vaccinated => there is no date */
                        writeString("NO",bufferSize,writeFd);
                        writeString((current->virusName).c_str(),bufferSize,writeFd);
                    }
                    current = current->next;        /* Go to the next virus */
                }
                writeString("STOPPED",bufferSize,writeFd);
            }

        }else if (command.compare("/travelRequest") == 0){          /* Command 1 - /travelRequest citizenID date countryFrom countryTo virusName */

            string virusName = readString(readFd, bufferSize);

            Date* date = VirusListCheckIdForVirus(virusListHead,citizenId,virusName);
            totalReq++; 

            if (date == NULL){
                writeString("NO",bufferSize,writeFd);
                rejectedReq++;
            }else{
                writeString("YES",bufferSize,writeFd);
                acceptedReq++;

                /* convert date to string */
                string dateStr = date->convertDateToStr();

                writeString(dateStr.c_str(), bufferSize, writeFd);
            }
        }
    }
    return 0;
}


/* ---------------------------------------------------------------- */


void handler_SIGINT_and_SIGQUIT(int sig){


    string logFileStr = "log_file." + to_string(getpid()) + ".txt";
    ofstream logFile;
    logFile.open(logFileStr);                                   /* Create and open the file */

    CountryListPrintInFile(countryListHead,logFile);            /* Print all the countries in the logFile */
    logFile << "TOTAL TRAVEL REQUESTS " << totalReq << endl;    /* Print total travel requests in the logfile */
    logFile << "ACCEPTED " << acceptedReq << endl;              /* Print accepted requests in the logfile */
    logFile << "REJECTED " << rejectedReq << endl;              /* Print rejected requests in the logfile */

    logFile.close();

    /* Delete the lists */
    VirusDeleteList(&virusListHead);
    CountryDeleteList(&countryListHead);
    CitizenDeleteList(&citizenListHead);
    exit(0);
}

void signal_SIGUSR1_Handler(int sig) {

    string country = inputDir + "/" + readString(readFd, bufferSize);

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
    writeInt(numOfViruses,writeFd);

    /* Send back to parent proccess (travelMonitor) numofViruses bloom filters (one bloom filter per virus) */
    for (int i = 0; i < numOfViruses; i++){

        string virusName = VirusListGetVirusName(virusListHead,i);
        writeString(virusName.c_str(), bufferSize, writeFd);                                         /* Send the virus name */
        writeBloom(VirusGetBloomArray(virusListHead,virusName),bufferSize,sizeOfBloom,writeFd);       /* Send the bloomFilter */
    }

    cout << "New vaccination records added successfully." << endl;
}