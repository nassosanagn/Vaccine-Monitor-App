#include "CitizenList.h"

using namespace std;

void CitizenListPrint(CitizenNode * head){

    CitizenNode * current = head;
    cout << "Printing the list: " << endl;
    int i = 1;
    while (current != NULL) {
        cout << "citizenID " << i++ << ": " << current->citizen.citizenId << endl;
        VInfoListPrint(current->citizen.citizenIDptr);
        current = current->next;
    }
    cout << endl;
}

/* Adding an item to the beginning of the list (pushing to the list) */
void CitizenListPush(CitizenNode ** head, Citizen cit, VaccinationInfo vacInfo){

    CitizenNode * new_node = new CitizenNode;
   
    /* Update data */
    new_node->citizen = cit;
    new_node->citizen.citizenIDptr = NULL;
    VInfoListPush(&(new_node->citizen.citizenIDptr),vacInfo);
    new_node->next = *head;

    /* Now the new CitizenNode will be the list's head */
    *head = new_node;
}

int CitizenListSearch(CitizenNode* head, int keyId, CitizenNode** found){

    CitizenNode* current = head;
    while (current != NULL){ 
        if (current->citizen.citizenId == keyId){     /* Get the addresss of CitizenNode */
            *found = current;
            return 1; 
        }
        current = current->next; 
    }

    *found = NULL;      /* If keyId is not in the list => return 0 and found = NULL */
    return 0;
}

/* Return the number of nodes = citizens in list */
int CitizenListCount(CitizenNode* head){

    CitizenNode* current = head;  
    int counter = 0; 
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  

    return counter;  
}

/* Return the number of people that are from country "countryName" */
int CitizenListCount(CitizenNode* head, string countryName){

    CitizenNode* current = head;  
    int counter = 0; 

    while (current != NULL) {
        if (countryName.compare(*(current->citizen.country)) == 0)
            counter++;  
        current = current->next;  
    }  

    return counter;  
}  

/* Return the number of people in each age group that are from country "countryName" */
void CitizenListCountByAge(CitizenNode* head, string countryName, float ageGroup[4]){

    CitizenNode* current = head;  
    int age;

    for (int i = 0; i < 4; i++)         /* Initialize each age group */
        ageGroup[i] = 0;

    while (current != NULL){

        if (countryName.compare(*(current->citizen.country)) == 0){         /* Check if citizen is from country "countryName" */
            age = current->citizen.age;

            if ((age >= 0) && (age <= 20))              /* Find the age group of the citizen */
                ageGroup[0]++;
            else if ((age >= 20) && (age <= 40))
                ageGroup[1]++;
            else if ((age >= 40) && (age <= 60))
                ageGroup[2]++;
            else
                ageGroup[3]++;
        }
        current = current->next;  
    } 
}  

/* Delete all nodes => delete list */
void CitizenDeleteList(CitizenNode** head){

    CitizenNode* current = *head;
    CitizenNode* temp = NULL;

    while (current != NULL) { 
        temp = current;
        current = current->next;
        VInfoDeleteList(&(temp->citizen.citizenIDptr));         /* For each node delete the virus info list */
        delete temp;
    } 
}