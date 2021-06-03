#include "requestsList.h"


/* Adding an item to the beginning of the list (pushing to the list) */
void RequestListPush(RequestNode ** head, Date requestsDate, string countryName){

    RequestNode * new_node = new(RequestNode);
   
    /* Update data */
    new_node->requestsDate = requestsDate;
    new_node->country = countryName;
    new_node->next = *head;
    new_node->requestsCounter++;

    /* Now the new RequestNode will be the list's head */
    *head = new_node;
}

/* Counts no. of nodes in linked list */
int RequestListCount(RequestNode* head){

    RequestNode* current = head;  
    int counter = 0; 
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  

    return counter;  
} 

int RequestListCount(RequestNode * head, Date date1, Date date2){

    int counter = 0;
    RequestNode * current = head;

    int i = 1;
    while (current != NULL){
        
        if (((current->requestsDate.compareDates(date1)) >= 0) && ((current->requestsDate.compareDates(date2)) <= 0)){
            counter++;
        }
        current = current->next;
    }
    return counter;
}

int RequestListCount(RequestNode * head, string countryName, Date date1, Date date2){

    int counter = 0;
    RequestNode * current = head;

    int i = 1;
    while (current != NULL){
        
        if (((current->requestsDate.compareDates(date1)) >= 0) && ((current->requestsDate.compareDates(date2)) <= 0)){
            if (current->country.compare(countryName) == 0)
                counter++;
        }
        current = current->next;
    }
    return counter;
}

void RequestListDelete(RequestNode** head){

    RequestNode* current = *head;  // Initialize current
    RequestNode* temp = NULL;

    while (current != NULL) { 
        temp = current;
        current = current->next;
        delete temp;
    } 
}