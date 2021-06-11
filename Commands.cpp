#include "Commands.h"

int travelRequest(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[], BloomNode* bloomListHead, NumOfRequests* req);      /* Command 1 */
int travelStats(string InputWords[], int numMonitors, Info monitorInfo[], BloomNode* bloomListHead);                                                  /* Command 2 */
int addVaccinationRecords(string InputWords[], int numMonitors, int socketBufferSize , Info monitorInfo[], BloomNode* bloomListHead);                 /* Command 3 */
int searchVaccinationStatus(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[]);                                          /* Command 4 */                             
int exit(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[], BloomNode* bloomListHead);                                   /* Command 5 */   

int Commands(int bloomSize, int numMonitors, int socketBufferSize, Info monitorInfo[], BloomNode* bloomListHead, NumOfRequests* req){

  string InputWords[1024];
  string input, word;
  
  cout << endl << "Enter a command: ";
  getline(cin, input);
  cout << endl;
  cin.clear();
  
  istringstream words(input);           /* Split the input into words */

  int i = 0;                            /* Gen each word into the array */
  while (words >> word)
    InputWords[i++] = word;
  
  if (InputWords[0].compare("/travelRequest") == 0){                /* Command 1 - /travelRequest citizenID date countryFrom countryTo virusName */

    travelRequest(InputWords,numMonitors,socketBufferSize,monitorInfo,bloomListHead,req); 
  
  }else if (InputWords[0].compare("/travelStats") == 0){                   /* Command 2 - /travelStats virusName date1 date2 [country] */

    travelStats(InputWords, numMonitors, monitorInfo,bloomListHead);

  }else if (InputWords[0].compare("/addVaccinationRecords") == 0){            /* Command 3 - /addVaccinationRecords country */

    addVaccinationRecords(InputWords,numMonitors,socketBufferSize,monitorInfo,bloomListHead);

  }else if (InputWords[0].compare("/searchVaccinationStatus") == 0){          /* Command 4 - /searchVaccinationStatus citizenID */

    searchVaccinationStatus(InputWords,numMonitors,socketBufferSize,monitorInfo);

  }else if (InputWords[0].compare("/exit") == 0){                             /* Command 5 - /exit */

    return exit(InputWords,numMonitors,socketBufferSize ,monitorInfo,bloomListHead);
  
  }else{                                                            /*  ERROR in commands */
    cout << "ERROR this command is not valid" << endl; }

  return 0;
}

/* Command 1 - /travelRequest citizenID date countryFrom countryTo virusName */
int travelRequest(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[], BloomNode* bloomListHead, NumOfRequests* req){
  
  int citizenId = stoi(InputWords[1]);
  string virusName = InputWords[5];

  if (BloomListSearch(bloomListHead,virusName) == false){         /* Check if virusName exists in the list => or error */
    cout << "There is no virus called: " << virusName << endl;
    return 0;
  }

  Date travelDate;
  travelDate.getDateFromStr(InputWords[2]);
  
  if (BloomSearchIdInBloom(bloomListHead,InputWords[1],virusName)){       /* If it exists in bloom */

    int foundFlag = -1;

    for (int i = 0; i < numMonitors; i++){
      if (CountryListSearch(monitorInfo[i].countryListHead,InputWords[3])){     /* Find which monitor has the country called "countryFrom" */

        foundFlag = 1;

        sendString(monitorInfo[i].socketFd, "/travelRequest", socketBufferSize);

        sendInt(monitorInfo[i].socketFd, citizenId);                               /* Send citizenId */
        sendString(monitorInfo[i].socketFd, virusName.c_str(),socketBufferSize);         /* Send virusName */

        string isVaccinated = readString(monitorInfo[i].socketFd,socketBufferSize);       /* Get the answer (if the citizen is vaccinated or not) */

        if (isVaccinated.compare("YES") == 0){                                  /* If citizen is vaccinated */

          string dateStr = readString(monitorInfo[i].socketFd, socketBufferSize);
          Date vaccinationDate;
          vaccinationDate.getDateFromStr(dateStr);

          if (vaccinationDate.lessThan6Months(travelDate)){       /* If citizen got vaccinated in the last 6 months  */

            cout << "REQUEST ACCEPTED – HAPPY TRAVELS" << endl;
            req->acceptedReq = req->acceptedReq + 1;
            BloomListAddAcceptReq(bloomListHead,virusName,travelDate,InputWords[3]);

          }else{                                              /* If citizen got vaccinated but not in the last 6 months */

            cout << "REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE" << endl;
            req->rejectedReq = req->rejectedReq + 1;
            BloomListAddRejectReq(bloomListHead,virusName,travelDate,InputWords[3]);
          }

        }else if(isVaccinated.compare("NO") == 0){             /* If citizen is not vaccinated */

          cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED" << endl;
          req->rejectedReq = req->rejectedReq + 1;
          BloomListAddRejectReq(bloomListHead,virusName,travelDate,InputWords[3]);
        }
        break;
      }
    }

    if (foundFlag == -1){
      cout<< "There is no country called: " << InputWords[3] << endl;
      return 0;
    }

  }else{
    cout << "REQUEST REJECTED – YOU ARE NOT VACCINATED" << endl;

    req->rejectedReq = req->rejectedReq + 1;
    BloomListAddRejectReq(bloomListHead,virusName,travelDate,InputWords[3]);
  }

  BloomListAddTotalReq(bloomListHead,virusName,travelDate,InputWords[3]);
  req->totalReq = req->totalReq + 1;
  return 1;
}
/* Command 2 - /travelStats virusName date1 date2 [country] */
int travelStats(string InputWords[], int numMonitors, Info monitorInfo[], BloomNode* bloomListHead){

  if ((InputWords[1].compare("\0") == 0) || (InputWords[2].compare("\0") == 0) || (InputWords[3].compare("\0") == 0) || (InputWords[5].compare("\0") != 0)){
    cout << "ERROR in command /travelStats" << endl;
    return 0;
  }

  string virusName = InputWords[1];

  if (BloomListSearch(bloomListHead,virusName) == false){         /* Check if virusName exists in the list */
    cout << "There is no virus called: " << virusName << endl;
    return 0;
  }

  Date date1;
  Date date2;
  date1.getDateFromStr(InputWords[2]);
  date2.getDateFromStr(InputWords[3]);

  int totalReqNum, acceptedReqNum, rejectedReqNum;      /* Requests counter */

  if (InputWords[4].compare("\0") != 0){          /* If a country was given as an argument */

    int foundFlag = -1;
    string country = InputWords[4];

    /* Check if country exists in the list with the countries */
    for (int i = 0; i < numMonitors; i++)
      if (CountryListSearch(monitorInfo[i].countryListHead,country))
        foundFlag = 1;
    
    if (foundFlag == -1){
      cout << "There is no country called: " << country << endl;
      return 0;
    }
    totalReqNum    = BloomListGetTotalRequests(bloomListHead, virusName, country, date1, date2);
    acceptedReqNum = BloomListGetAcceptRequests(bloomListHead,virusName, country, date1, date2);
    rejectedReqNum = BloomListGetRejectRequests(bloomListHead,virusName, country, date1, date2);

  }else{

    totalReqNum    = BloomListGetTotalRequests(bloomListHead, virusName, date1, date2);
    acceptedReqNum = BloomListGetAcceptRequests(bloomListHead,virusName, date1, date2);
    rejectedReqNum = BloomListGetRejectRequests(bloomListHead,virusName, date1, date2);
  }

  /* Print the number of requests */
  cout << "TOTAL REQUESTS " << totalReqNum << endl;
  cout << "ACCEPTED " << acceptedReqNum << endl;
  cout << "REJECTED " << rejectedReqNum << endl;
  return 1;
}
/* Command 3 - /addVaccinationRecords country */
int addVaccinationRecords(string InputWords[], int numMonitors, int socketBufferSize , Info monitorInfo[], BloomNode* bloomListHead){ 

  if ((InputWords[1].compare("\0") == 0) || (InputWords[2].compare("\0") != 0)){    /* There must be exactly 2 arguments */
    cout << "ERROR in command /addVaccinationRecords" << endl;
    return 0;
  }

  int foundFlag = -1;                /* A flag that indicates if country exists */
  string country = InputWords[1];

  for (int i = 0; i < numMonitors; i++){
  
    if (CountryListSearch(monitorInfo[i].countryListHead,country)){     /* Find which monitor has the country */

      foundFlag = 1;

      sendString(monitorInfo[i].socketFd, "/addVaccinationRecords", socketBufferSize);    
      sendString(monitorInfo[i].socketFd, country.c_str(),socketBufferSize);     /* Send county's name to monitor */

      int numOfViruses = readInt(monitorInfo[i].socketFd);              /* Get the number of viruses (equals with the number of bloom filters) */
      int sizeOfBloom = bloomListHead->bloomFilter->getBloomSize();
      char* tempBloom;

      for (int j = 0; j < numOfViruses; j++){             /* How many bloom filters to read */
        
        string virusNameStr = readString(monitorInfo[i].socketFd, socketBufferSize);                /* Get the virus name */
        tempBloom = readBloom(monitorInfo[i].socketFd, sizeOfBloom, socketBufferSize);              /* Get the bloom filter */

        /* Update the list with the bloom filters */
        if (!(BloomListSearch(bloomListHead, virusNameStr)))                        /* If virusName is not already in the list => add it */
            BloomListPush(&bloomListHead, virusNameStr, tempBloom, sizeOfBloom);
        else
            BloomListMergeBlooms(bloomListHead, virusNameStr, tempBloom);
        
        delete tempBloom;  
      }
    }
  }

  if (foundFlag == -1)
    cout << "There is no country called: " << country << endl;

  return 1;
}
/* Command 4 - /searchVaccinationStatus citizenID */
int searchVaccinationStatus(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[]){

  if ((InputWords[1].compare("\0") == 0) || (InputWords[2].compare("\0") != 0)){
    cout << "ERROR in command /searchVaccinationStatus" << endl;
    return 0;
  }

  int foundFlag = -1;                        /* A flag that indicates if citizen with id "citizenId" exists */
  int citizenId = stoi(InputWords[1]);

  for (int i = 0; i < numMonitors; i++){

    sendString(monitorInfo[i].socketFd, "/searchVaccinationStatus", socketBufferSize);    
    sendInt(monitorInfo[i].socketFd, citizenId);

    foundFlag = readInt(monitorInfo[i].socketFd);

    if (foundFlag == 1){    /* this is the monitor (with index i) that contains citizen with id "citizenId" */

      string firstName = readString(monitorInfo[i].socketFd, socketBufferSize);
      string lastName = readString(monitorInfo[i].socketFd, socketBufferSize);
      string country = readString(monitorInfo[i].socketFd, socketBufferSize);
      int age = readInt(monitorInfo[i].socketFd);

      cout << endl << citizenId << " " << firstName << " " << lastName << " " << country << endl;
      cout << "AGE " << age << endl;
  
      while(1){

        string isVaccinated = readString(monitorInfo[i].socketFd, socketBufferSize);

        if (isVaccinated.compare("STOPPED") == 0)
          break;
        
        if (isVaccinated.compare("YES") == 0){                                       /* If citizen is in Vaccinated Skip List */

          string virusName = readString(monitorInfo[i].socketFd, socketBufferSize);
          cout << virusName << " VACCINATED ON ";
          string dateStr = readString(monitorInfo[i].socketFd, socketBufferSize);
          Date vaccinationDate;
          vaccinationDate.getDateFromStr(dateStr);
          vaccinationDate.printDate();
          cout << endl;

        }else if (isVaccinated.compare("NO") == 0){                                     /* If citizen is in NotVaccinated Skip List */
          string virusName = readString(monitorInfo[i].socketFd, socketBufferSize);
          cout << virusName << " NOT YET VACCINATED" << endl;
        }
      }
      break;
    }
  }
  if (foundFlag == -1)
    cout << "There is no citizen with Id: " << citizenId << endl;

  return 1;
}
/* Command 5 - /exit */
int exit(string InputWords[], int numMonitors, int socketBufferSize, Info monitorInfo[], BloomNode* bloomListHead){

  /* There must be no other arguments => only eixt */
  if ((InputWords[1].compare("\0") != 0)){
    cout << "ERROR in command /exit" << endl;
    return 0;
  }

  for (int i = 0; i < numMonitors; i++)
    sendString(monitorInfo[i].socketFd, "/exit", socketBufferSize);    

  BloomListDelete(&bloomListHead);

  for (int i = 0; i < numMonitors; i++)
    CountryDeleteList(&(monitorInfo[i].countryListHead));

  return 1;
}
