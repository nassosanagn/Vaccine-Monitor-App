#ifndef VACCINATION_INFO_LIST_H_
#define VACCINATION_INFO_LIST_H_

#include "VirusList.h"
#include "Date.h"

typedef struct VaccinationInfo{     /* Every virus has one bloom filter and 2 skip lists */
    string isVaccinated;
    Date* dateVaccinated;
    struct VirusNode* virusName;
}VaccinationInfo;

typedef struct VInfoNode{
   VaccinationInfo vacInfo;
   struct VInfoNode* next;
}VInfoNode;

void VInfoListPrint(VInfoNode * head);

/* Add a new VInfoNode in the beginning of the list */
void VInfoListPush(VInfoNode ** head, VaccinationInfo vacInfo);

/* Return "true" if "virusName" is in VirusInfo List or "false" if it isn't */
bool VInfoListSearch(VInfoNode* head, string virusName);

/* Return the address of VInfoNode if "virusName" is in VirusInfo List or NULL if it isn't */
VInfoNode* VInfoListSearch2(VInfoNode* head, string virusName);

/* Return the number of nodes = different viruses info in the list */
int VInfoListCount(VInfoNode* head);

/* Delete all nodes in the list */
void VInfoDeleteList(VInfoNode** head);

#endif /* VIRUS_LIST_H_ */