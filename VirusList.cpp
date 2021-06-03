#include "VirusList.h"

using namespace std;

void VirusListPrint(VirusNode * head){

    VirusNode * current = head;
    cout << "Printing the list with the viruses: " << endl;
    int i = 1;
    while (current != NULL) {
        cout << "Virus name " << i++ << ": " << current->virusName << endl;
        current = current->next;
    }
    cout << endl;
}

/* Add an element to the beginning of the list (pushing to the list) */
void VirusListPush(VirusNode ** head, string virusName, int bloomFilterSize){

    VirusNode * newNode = new VirusNode;
   
    /* Update data */
    newNode->virusName = virusName;
    newNode->bloomFilter = new BloomFilter(bloomFilterSize);
    newNode->Vaccinated = new SkipList(MAX_HEIGHT);
    newNode->NotVaccinated = new SkipList(MAX_HEIGHT);
    newNode->next = *head;

    /* Now the new VirusNode will be the list's head */
    *head = newNode;
}

Date* VirusListCheckIdForVirus(VirusNode* head, int keyId, string virusName){

    VirusNode* current = head;
    CitizenNode* cit;
    VInfoNode* found;

    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0){            // find the virus

            if (current->Vaccinated->SkipListSearch(keyId,&cit)){        /* If person is vaccinated */
                found = VInfoListSearch2(cit->citizen.citizenIDptr,current->virusName);
                return found->vacInfo.dateVaccinated;
            }
            
            return NULL; 
        } 
        current = current->next; 
    } 
    return NULL;
}

int VirusSearchIdInBloom(VirusNode* head, string citizenId, string virusName){

   VirusNode* current = head;
   
   while (current != NULL){         /* For each virus until we find virus called "virusName" */

      if (current->virusName.compare(virusName) == 0)
        return current->bloomFilter->BloomFilterSearch(citizenId);      /* Search virusName's bloom filter for citizenId => Return 1 or 0 */
      
      current = current->next;      /* Go to the next virus */
    }

   cout << "There is no virus called: " << virusName << endl;
   return 0;
}

/* Return the address of VirusNode if "virusName" is in Virus List or NULL if it isn't */
VirusNode* VirusGetNode(VirusNode* head, string virusName){
    
    VirusNode* current = head;
   
    while (current != NULL){

        if (current->virusName.compare(virusName) == 0)
            return current;
        
        current = current->next;
    }

   cout << "There is no virus called: " << virusName << endl;
   return NULL;
}

int VirusPrintNotVacList(VirusNode* head, string virusName){

   VirusNode* current = head;

   while (current != NULL){ 

      if (current->virusName.compare(virusName) == 0){
        current->NotVaccinated->SkipLevelPrint(0);
        return 1;
      }

      current = current->next; 
   }
   return 0;
}

/* Return "true" if "virusName" is in Virus List or "false" if it isn't */
bool VirusListSearch(VirusNode* head, string virusName){

    VirusNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0) 
            return true; 
        current = current->next; 
    } 
    return false;
}

char* VirusGetBloomArray(VirusNode* head, string virusName){

    VirusNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0) {
            return current->bloomFilter->getBloom();
        }
        current = current->next; 
    } 
    return NULL;
}

string VirusListGetVirusName(VirusNode* head, int index){

    if (index > VirusListCount(head)){
        perror("Out on limits");
        exit(1);
    }

    VirusNode* current = head;

    for (int i = 1; i <= index; i++){
        current = current->next; 
    }

    if (current != NULL)
        return current->virusName;
    else
        return "";
}



/* Return the number of nodes = different viruses in the list */
int VirusListCount(VirusNode* head){

    VirusNode* current = head;  
    int counter = 0; 
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  
    return counter;  
}  

/* Delete all nodes => delete list */
void VirusDeleteList(VirusNode** head){

    VirusNode* current = *head;
    VirusNode* temp = NULL;

    while (current != NULL) {

        temp = current;
        current = current->next;

        delete temp->bloomFilter;        /* Delete the bloom filter */
        delete temp->Vaccinated;         /* Delete vaccinated skip list */
        delete temp->NotVaccinated;      /* Delete not vaccinated skip list */
        delete temp;                     /* Delete VirusNode */
    } 
}
