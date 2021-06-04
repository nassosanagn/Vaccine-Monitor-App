#include "SocketFunctions.h"

string readString(int socketFd, int bufferSize){

    int strSize;

    if (read(socketFd , &strSize, sizeof(int)) == -1){
        perror("Couldnt read strSize");
        exit(EXIT_FAILURE);
    }
    
    char temp[strSize];
    memset(temp, '\0', sizeof(strSize));
    int counter = 0;

    for (int i = 0; i < strSize/bufferSize; i++){

        if (read(socketFd , &(temp[counter]) ,bufferSize) == -1){
            perror("Couldnt read string");
            exit(EXIT_FAILURE);
        }
        counter += bufferSize;
    }

    if (counter < strSize){                      /* write the remaining bytes */
                   
        int tempBuff = strSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (read(socketFd , &(temp[counter]),tempBuff) == -1){
            perror("Couldnt read string");
            exit(EXIT_FAILURE);
        }
    }

    temp[strSize] = '\0';
    string retStr(temp);
    return retStr;
}

int readInt(int socketFd){
    
    int temp;

    if (read(socketFd, &temp, sizeof(int)) == -1){ 
        perror("Couldnt read temp");
        exit(EXIT_FAILURE);
    }
    return temp;
}

int sendString(int socketFd, const char* strToSend, int bufferSize){

    int strSize = strlen(strToSend);

    if (send(socketFd , &strSize , sizeof(int), 0) == -1){
        perror("Couldnt write strSize");
        exit(EXIT_FAILURE);
    }

    int counter = 0;

    for (int i = 0; i < strSize/bufferSize; i++){

        if (send(socketFd , &(strToSend[counter]) , bufferSize, 0 ) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
        counter += bufferSize;
    }

    if (counter < strSize){     /* write the remaining bytes */
            
        int tempBuff = strSize - counter;                       /* tempBuff will always be smaller that the bufferSize */

        if (send(socketFd , &(strToSend[counter]) , tempBuff, 0) == -1){
            perror("Couldnt write strSize");
            exit(EXIT_FAILURE);
        }
    }

    return 1;
}

void sendInt(int socketFd, int x){
    
    int temp = x;

    if (send(socketFd, &temp, sizeof(int), 0) == -1){  
        perror("Couldnt write strSize");
        exit(EXIT_FAILURE);
    }
}

char* readBloom(int socketFd, int bloomSize, int bufferSize){

    char* temp = new char[bloomSize + 1];
    memset(temp,'\0', bloomSize + 1);

    int counter = 0;
    for (int i = 0; i < bloomSize/bufferSize; i++){
            
        if (read(socketFd, &(temp[counter]), bufferSize) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
      counter += bufferSize;
    }
    if (counter < bloomSize){     /* write the remaining bytes */
                   
        int tempBuff = bloomSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (read(socketFd, &(temp[counter]), tempBuff) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
    }

    temp[bloomSize + 1] = '\0';
    return temp;
}

void sendBloom(int socketFd, const char* strToSend, int bloomSize, int bufferSize){

    int counter = 0;
    for (int i = 0; i < bloomSize/bufferSize; i++){

        if (send(socketFd, &(strToSend[counter]), bufferSize, 0) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
        counter += bufferSize;
    }

    if (counter < bloomSize){     /* write the remaining bytes */
            
        int tempBuff = bloomSize - counter;       /* tempBuff will always be smaller that the bufferSize */

        if (send(socketFd, &(strToSend[counter]), tempBuff, 0) == -1){
            perror("Couldnt write bloomSize");
            exit(EXIT_FAILURE);
        }
    }
}

/*---------------------------------------------------------------- */