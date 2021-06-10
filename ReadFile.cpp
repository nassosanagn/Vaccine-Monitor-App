#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

#include "ReadFile.h"

int readFile(string fileName, int bloomSize, CitizenNode** CitizenListHead, VirusNode** VirusListHead, CountryNode** CountryListHead){

  int counter;      /* count the words for every line from "citizenRecordsFile" */
  int i = 0;
  bool isVaccinated, errorFlag;

  string line,word,strId;

  ifstream file;

  CitizenNode* found;
  Citizen citizen;
  VaccinationInfo vacInfo;

  file.open (fileName);   /* Open the file "citizenRecordsFile" */

  if (file.fail()){
    cout << "Error: Couldn't open the file! " << endl;
    exit(EXIT_FAILURE);
  }

  while (getline (file, line)) {      /* For every line in the file */
  
    /* Split the line into words */ 
    istringstream words(line);
    counter = 1;
    errorFlag = 0;

    while (words >> word){         /* For each word in the line */

      if (counter == 1){                /* 1st word == citizenID */
        citizen.citizenId = stoi(word);
        strId = word;
    
      }else if (counter == 2){           /* 2nd word == firstName */
        citizen.firstName = word;
       
      }else if (counter == 3){           /* 3rd word == lastName */
        citizen.lastName = word;
       
      }else if(counter == 4){           /* 4th word == country */

        /* Create the list with countries */
        if (!(CountryListSearch(*CountryListHead, word))){     /* If country is not already in the list => add it (in the beginning) */
          CountryListPush(CountryListHead,word);
        }

        /* Add a pointer to the country in Citizen List */
        citizen.country = CountryListSearch2(*CountryListHead, word);
        
      }else if (counter == 5){           /* 5th word == age */
        citizen.age = stoi(word);
      
      }else if (counter == 6){           /* 6th word == virusName */

        /* Create the list with the viruses */
        if (!(VirusListSearch(*VirusListHead, word))){     /* If virus is not already in the list => add it (in the beginning) */
          VirusListPush(VirusListHead, word, bloomSize);
        }
        vacInfo.virusName = VirusGetNode(*VirusListHead, word);

      }else if (counter == 7){           /* 7th word == if person is vaccinated */
        vacInfo.isVaccinated = word;

        if (vacInfo.isVaccinated.compare("NO") == 0){
          isVaccinated = false;
          vacInfo.dateVaccinated = NULL;          /* If citizen isn't vaccinated => there is no vaccination date */
          
        }else if (vacInfo.isVaccinated.compare("YES") == 0){
          isVaccinated = true;
          vacInfo.dateVaccinated = new Date();
        }
      
      }else if (counter == 8){                   /* 8th word == dateVaccinated */
      
        if (!(word.empty()) && isVaccinated == false){                /* If answer == "NO" but there is a date => Error */
          cout << endl << "ERROR IN RECORD " << i + 1 << " NO WITH DATE" << endl;
          errorFlag = 1;
          break;
        }
        vacInfo.dateVaccinated->getDateFromStr(word);
      }
      counter++;
    }

    if (errorFlag)    /* Ignore record => Go to the next line */
      continue;
    
    /* Check if CitizenId already exists */
    if (CitizenListSearch(*CitizenListHead,citizen.citizenId,&found)){ 

      /* Check to see if other info matches e.g. firstname, lastname, age,... */
      /* If something doesn't match => ignore the record */
      if ((citizen.firstName.compare(found->citizen.firstName) != 0) || (citizen.lastName.compare(found->citizen.lastName) != 0) ||  (citizen.country->compare(*(found->citizen.country)) != 0) || (citizen.age != found->citizen.age)){            
        /* Same id but different citizen information */
        i++;
        delete vacInfo.dateVaccinated;      /* Record doesn't go in Vaccination Info List => delete date created */
        continue;                           /* Ignore the record */
      }      

      /* If all citizen info matches => check virus */                     
      /* Check if the same virus already exists for the citizen with this citizenId*/
      if (VInfoListSearch(found->citizen.citizenIDptr,vacInfo.virusName->virusName)){ /* Same id & same virus => ignore record */

        delete vacInfo.dateVaccinated;      /* Record doesn't go in Vaccination Info List => delete date created */
        continue;                           /* Ignore the record */

      }else{                                                        /* Same id but different virus */
        VInfoListPush(&(found->citizen.citizenIDptr), vacInfo);   /* Add to vaccination Info list for already existing citizen */
      }

    }else{    /* If citizen doesn't exist in Citizen List */

      CitizenListPush(CitizenListHead, citizen, vacInfo);      /* Insert in Citizen List */
    }

    if (vacInfo.isVaccinated.compare("YES") == 0){     /* Insert in Bloom Filter and in vaccinated persons list */
      
      vacInfo.virusName->bloomFilter->BloomFilterInsert(strId);                            /* Insert Bloom filter */
      vacInfo.virusName->Vaccinated->SkipListInsert(citizen.citizenId, *CitizenListHead);

    }else{                /* Insert in not vaccinated persons list */
      vacInfo.virusName->NotVaccinated->SkipListInsert(citizen.citizenId, *CitizenListHead);
    }
    i++;
  }

  file.close();   /* Close the file */
  return i;
}