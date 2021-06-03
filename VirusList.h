#ifndef VIRUS_LIST_H_
#define VIRUS_LIST_H_

#include "BloomFilter.h"
#include "SkipList.h"

typedef struct VirusNode{     /* Every virus has one bloom filter and 2 skip lists */
   string virusName;
   struct BloomFilter* bloomFilter;
   struct SkipList* Vaccinated;
   struct SkipList* NotVaccinated;
   struct VirusNode* next;
}VirusNode;

void VirusListPrint(VirusNode * head);

/* Add a new virusNode in the beginning of the list */
void VirusListPush(VirusNode ** head, string virusName, int bloomFilterSize);

/* Return the address of VirusNode if "virusName" is in Virus List or NULL if it isn't */
VirusNode* VirusGetNode(VirusNode* head, string virusName);

/* Return "true" if "virusName" is in Virus List or "false" if it isn't */
bool VirusListSearch(VirusNode* head, string virusName);

/* Return the number of nodes = different viruses in the list */
int VirusListCount(VirusNode* head);

/* Delete all nodes in the list */
void VirusDeleteList(VirusNode** head);

/* Used for Command 1 "/vaccineStatusBloom citizenID virusName" */
/* Return 1 if citizenId is in the Bloom filter of virus "virusName" */
int VirusSearchIdInBloom(VirusNode* head, string citizenId, string virusName);

/* Used for Command 3 "/vaccineStatus citizenID" */
int CheckIdForEachVirus(VirusNode* head, int keyId);

/* Used for Command 8 "/list-nonVaccinated-Persons virusName" */
int VirusPrintNotVacList(VirusNode* head, string virusName);

char* VirusGetBloomArray(VirusNode* head, string virusName);
string VirusListGetVirusName(VirusNode* head, int index);

Date* VirusListCheckIdForVirus(VirusNode* head, int keyId, string virusName);

#endif /* VIRUS_LIST_H_ */