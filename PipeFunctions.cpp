#include "PipeFunctions.h"

string readString(int readFd, int bufferSize){

    int strSize = 0;

    if (read(readFd, &strSize, sizeof(int)) == -1){
        perror("Couldnt write strSize");
        exit(EXIT_FAILURE);
    }

    char temp[strSize];
    int counter = 0;

    for (int i = 0; i < strSize/bufferSize; i++){
            
        if (read(readFd, &(temp[counter]), bufferSize) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
      counter += bufferSize;
    }

    if (counter < strSize){     /* write the remaining bytes */
                   
        int tempBuff = strSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (read(readFd, &(temp[counter]), tempBuff) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
    }

    temp[strSize] = '\0';
    string retStr(temp);
    return retStr;
}

void writeString(const char* strToSend, int bufferSize, int writeFd){

    int strSize = strlen(strToSend);

    if (write(writeFd, &strSize, sizeof(int)) == -1){
        perror("Couldnt write strSize");
        exit(EXIT_FAILURE);
    }

    int counter = 0;

    for (int i = 0; i < strSize/bufferSize; i++){

        if (write(writeFd, &(strToSend[counter]), bufferSize) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
        counter += bufferSize;
    }

    if (counter < strSize){     /* write the remaining bytes */
            
        int tempBuff = strSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (write(writeFd, &(strToSend[counter]), tempBuff) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
    }
}

int readInt(int readFd){
    
    int temp;

    if (read(readFd, &temp, sizeof(int)) == -1){ 
        perror("Couldnt read temp");
        exit(EXIT_FAILURE);
    }
    return temp;
}

void writeInt(int x, int writeFd){
    
    int temp = x;

    if (write(writeFd, &temp, sizeof(int)) == -1){  
        perror("Couldnt write strSize");
        exit(EXIT_FAILURE);
    }
}

char* readBloom(int readFd, int bufferSize, int bloomSize){

    char* temp = new char[bloomSize + 1];
    memset(temp,'\0', bloomSize + 1);

    int counter = 0;
    for (int i = 0; i < bloomSize/bufferSize; i++){
            
        if (read(readFd, &(temp[counter]), bufferSize) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
      counter += bufferSize;
    }
    if (counter < bloomSize){     /* write the remaining bytes */
                   
        int tempBuff = bloomSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (read(readFd, &(temp[counter]), tempBuff) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
    }

    temp[bloomSize + 1] = '\0';
    return temp;
}

void writeBloom(const char* strToSend, int bufferSize, int bloomSize, int writeFd){

    int counter = 0;
    for (int i = 0; i < bloomSize/bufferSize; i++){

        if (write(writeFd, &(strToSend[counter]), bufferSize) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
        counter += bufferSize;
    }

    if (counter < bloomSize){     /* write the remaining bytes */
            
        int tempBuff = bloomSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (write(writeFd, &(strToSend[counter]), tempBuff) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
    }
}