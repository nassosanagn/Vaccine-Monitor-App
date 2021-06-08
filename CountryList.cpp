#include "CountryList.h"

void CountryListPrint(CountryNode * head){

    CountryNode * current = head;
    cout << "Printing the list: " << endl;
    int i = 1;
    while (current != NULL) {
        cout << "Country " << i++ << ": " << current->country << endl;
        current = current->next;
    }
    
    cout << endl;
}

void CountryListPrintInFile(CountryNode * head, ofstream& logFile){

    CountryNode * current = head;

    int i = 1;
    while (current != NULL) {
        logFile << current->country << endl;
        current = current->next;
    }
}

/* Adding an item to the beginning of the list (pushing to the list) */
void CountryListPush(CountryNode ** head, string country){

    CountryNode * new_node = new(CountryNode);
   
    /* Update data */
    new_node->country = country;
    new_node->next = *head;

    /* Now the new CountryNode will be the list's head */
    *head = new_node;
}


bool CountryListSearch(CountryNode* head, string countryName){

    CountryNode* current = head;  // Initialize current 
    while (current != NULL) 
    { 
        if (current->country.compare(countryName) == 0) 
            return true; 
        current = current->next; 
    } 
    return false;
}


string* CountryListSearch2(CountryNode* head, string countryName){

    CountryNode* current = head;  // Initialize current 
    while (current != NULL) 
    { 
        if (current->country.compare(countryName) == 0) 
            return &(current->country); 
        current = current->next; 
    } 
    return NULL;
}

/* Counts no. of nodes in linked list */
int CountryListCount(CountryNode* head){

    CountryNode* current = head;  
    int counter = 0; 
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  

    return counter;  
}  

void CountryDeleteList(CountryNode** head){

    CountryNode* current = *head;  // Initialize current
    CountryNode* temp = NULL;

    while (current != NULL) { 
        temp = current;
        current = current->next;
        delete temp;
    } 
}

string getAllCountries(CountryNode* head){

    CountryNode* current = head;  
    string allCountries = "";
    
    while (current != NULL) {  

        allCountries = allCountries + " " + current->country;
        current = current->next;  
    }  

    return allCountries;  

}