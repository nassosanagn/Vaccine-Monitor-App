#include "bloomList.h"

using namespace std;

void BloomListPrint(BloomNode * head){

    BloomNode * current = head;
    cout << "Printing the list with the bloom filters: " << endl;
    int i = 1;
    while (current != NULL) {
        cout << "Virus name " << i++ << ": " << current->virusName << endl;
        current = current->next;
    }
    cout << endl;
}

void BloomListPush(BloomNode ** head, string virusName, char* bloomArray, int bloomFilterSize){
    BloomNode * newNode = new BloomNode;
   
    /* Update data */
    newNode->virusName = virusName;
    newNode->bloomFilter = new BloomFilter(bloomFilterSize,bloomArray);
    newNode->next = *head;

    /* Now the new BloomNode will be the list's head */
    *head = newNode;
}
/* Add an element to the beginning of the list (pushing to the list) */
void BloomListPush(BloomNode ** head, string virusName, int bloomFilterSize){

    BloomNode * newNode = new BloomNode;
   
    /* Update data */
    newNode->virusName = virusName;
    newNode->bloomFilter = new BloomFilter(bloomFilterSize);
    newNode->next = *head;

    /* Now the new BloomNode will be the list's head */
    *head = newNode;
}

/* Return "true" if "virusName" is in Virus List or "false" if it isn't */
bool BloomListSearch(BloomNode* head, string virusName){

    BloomNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0) 
            return true; 
        current = current->next; 
    } 
    return false;
}

void BloomListAddTotalReq(BloomNode* head, string virusName, Date reqDate, string countryName){

    BloomNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0){

            RequestListPush(&(current->totalReq), reqDate, countryName);
            return; 
        }
        current = current->next; 
    } 
}

void BloomListAddAcceptReq(BloomNode* head, string virusName, Date reqDate, string countryName){
    
    BloomNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0){

            RequestListPush(&(current->acceptedReq), reqDate, countryName);
            return; 
        }
        current = current->next; 
    }
}

void BloomListAddRejectReq(BloomNode* head, string virusName, Date reqDate, string countryName){
    
    BloomNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0){
            
            RequestListPush(&(current->rejectedReq), reqDate, countryName);
            return; 
        }
        current = current->next; 
    }
}

int BloomListGetTotalRequests(BloomNode* head,string virusName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->totalReq, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}

int BloomListGetAcceptRequests(BloomNode* head,string virusName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->acceptedReq, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}

int BloomListGetRejectRequests(BloomNode* head,string virusName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->rejectedReq, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}

/* ----------------------------------------------------------------------- */
int BloomListGetTotalRequests(BloomNode* head, string virusName, string countryName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->totalReq, countryName, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}

int BloomListGetAcceptRequests(BloomNode* head,string virusName, string countryName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->acceptedReq, countryName, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}

int BloomListGetRejectRequests(BloomNode* head,string virusName, string countryName, Date date1, Date date2){

    BloomNode* current = head;
    while (current != NULL) {
            
        if (current->virusName.compare(virusName) == 0){
            return RequestListCount(current->rejectedReq, countryName, date1, date2);
        }
        current = current->next; 
    }

    cout << "error there is no virus called: " << virusName << endl;
    return 0;
}
/* --------------------------------------------------------------------------- */

bool BloomListMergeBlooms(BloomNode* head, string virusName, char* bloomArray){

    BloomNode* current = head;
    while (current != NULL) {

        if (current->virusName.compare(virusName) == 0){
            current->bloomFilter->BloomFilterMerge(bloomArray);
            return true; 
        } 
        current = current->next; 
    } 
    return false;
}

int BloomSearchIdInBloom(BloomNode* head, string citizenId, string virusName){

   BloomNode* current = head;
   
   while (current != NULL){         /* For each virus until we find virus called "virusName" */

      if (current->virusName.compare(virusName) == 0){
        return current->bloomFilter->BloomFilterSearch(citizenId);      /* Search virusName's bloom filter for citizenId => Return 1 or 0 */
      }
      
      current = current->next;      /* Go to the next virus */
    }

   cout << "There is no virus called: " << virusName << endl;
   return 0;
}

/* Return the number of nodes = different viruses in the list */
int BloomListCount(BloomNode* head){

    BloomNode* current = head;  
    int counter = 0; 
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  
    return counter;  
}  

/* Delete all nodes => delete list */
void BloomListDelete(BloomNode** head){

    BloomNode* current = *head;
    BloomNode* temp = NULL;

    while (current != NULL) {

        temp = current;
        current = current->next;

        delete temp->bloomFilter;                /* Delete the bloom filter */
        
        RequestListDelete(&(temp->acceptedReq));
        RequestListDelete(&(temp->rejectedReq));
        RequestListDelete(&(temp->totalReq));

        // delete temp->acceptedReq;
        delete temp;                     /* Delete VirusNode */
    } 
}
