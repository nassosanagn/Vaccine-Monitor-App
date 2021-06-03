#include "VaccinationInfoList.h"

using namespace std;

void VInfoListPrint(VInfoNode * head){

    VInfoNode * current = head;
    cout << "Printing the list: " << endl;
    int i = 1;
    while (current != NULL) {
        cout << i++ << ": " << current->vacInfo.virusName->virusName;
        cout << " " << current->vacInfo.isVaccinated << " ";
        if (current->vacInfo.dateVaccinated != NULL)                /* If isVaccinated == "NO" => date == NULL => don't print it */
            current->vacInfo.dateVaccinated->printDate();
        cout << endl;
        current = current->next;
    }
    cout << endl;
}

/* Adding an item to the beginning of the list (pushing to the list) */
void VInfoListPush(VInfoNode ** head, VaccinationInfo vacInfo){

    VInfoNode * new_node = new VInfoNode;
   
    /* Update data */
    new_node->vacInfo = vacInfo;
    new_node->next = *head;

    /* Now the new VInfoNode will be the list's head */
    *head = new_node;
}

/* Return "true" if "virusName" is in VirusInfo List or "false" if it isn't */
bool VInfoListSearch(VInfoNode* head, string virusName){

    VInfoNode* current = head;

    while (current != NULL){ 
        if (current->vacInfo.virusName->virusName.compare(virusName) == 0) 
            return true; 
        current = current->next; 
    } 
    return false;
}

/* Return the address of VInfoNode if "virusName" is in VirusInfo List or NULL if it isn't */
VInfoNode* VInfoListSearch2(VInfoNode* head, string virusName){

    VInfoNode* current = head;
 
    while (current != NULL){ 
        if (current->vacInfo.virusName->virusName.compare(virusName) == 0)
            return current; 
        current = current->next; 
    } 
    return NULL;
}

int VInfoListCount(VInfoNode* head){

    VInfoNode* current = head;  
    int counter = 0;
     
    while (current != NULL) {  
        current = current->next;  
        counter++;  
    }  

    return counter;  
}  

void VInfoDeleteList(VInfoNode** head){

    VInfoNode* current = *head;
    VInfoNode* temp = NULL;
    int i = 0;

    while (current != NULL) { 
        temp = current;
        current = current->next;
        if (temp->vacInfo.dateVaccinated != NULL)           /* Delete dates => only if they exist */
            delete temp->vacInfo.dateVaccinated;
        delete temp;                                        /* Delete VInfoNode */
    } 
}