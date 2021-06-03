#ifndef CITIZEN_LIST_H_
#define CITIZEN_LIST_H_

#include <iostream>
#include <string>

#include "BloomFilter.h"
#include "VaccinationInfoList.h"

typedef struct Citizen{
    int citizenId;
    struct VInfoNode* citizenIDptr;   
    string firstName;
    string lastName;
    string* country;
    int age;
}Citizen;

typedef struct CitizenNode{
   Citizen citizen;
   struct CitizenNode* next;
}CitizenNode;

void CitizenListPrint(CitizenNode * head);

/* Add a new CitizenNode in the beginning of the list */
void CitizenListPush(CitizenNode ** head, Citizen cit, struct VaccinationInfo vacInfo);

/* Return "true" if "keyId" is in Citizen List or "false" if it isn't */
/* Also return the adrress of CitizenNode as an argument if keyId exists or NULL if it doesn't */
int CitizenListSearch(CitizenNode* head, int keyId, CitizenNode** found);

/* Return the number of nodes == citizens in list */
int CitizenListCount(CitizenNode* head);

/* Return the number of people that are from country "countryName" */
int CitizenListCount(CitizenNode* head, string countryName);

/* Return the number of people in each age group that are from country "country" */
void CitizenListCountByAge(CitizenNode* head, string countryName, float ageGroup[4]);

/* Delete all nodes in the list */
void CitizenDeleteList(CitizenNode** head);

#endif /* CITIZEN_LIST_H_ */