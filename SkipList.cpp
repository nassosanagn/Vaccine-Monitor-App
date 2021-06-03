#include "SkipList.h"

using namespace std;


/* ------------------------------ Skip List ---------------------------------------- */

SkipList::SkipList(int maxLevel){
    this->maxHeight = maxLevel;
    this->listHead = new SkipNode;
    this->listHead->citizenID = -999;

    this->listHead->next = new SkipNode*[MAX_HEIGHT];
    for (int i = 0; i < MAX_HEIGHT; i++){
        this->listHead->next[i] = NULL;
    }
}

SkipList::~SkipList(){

    SkipNode* current = listHead;
    SkipNode* temp;

    while (current != NULL){        /* For every skip list node at level 0 */

        temp = current;
        current = current->next[0];        /* Go to the next Node */    
        delete[] temp->next;            /* Delete pointers array */
        delete temp;                    /* Delete the SkipNode */
    }
}

SkipNode* SkipList::SkipListInsert(int id, CitizenNode* head){

    SkipNode* current;
    SkipNode* prev = NULL;

    bool flag;
    int levels = getRandomLevel();      /* Get a random level to insert the element from level 0 to level with index "levels" */

    /* Create the new node */
    SkipNode* new_node = new SkipNode;

    /* Initialize the new node */
    new_node->citizenID = id;
    CitizenListSearch(head,id,&(new_node->citNode));
    new_node->next = new SkipNode*[MAX_HEIGHT];

    for (int i = 0; i < MAX_HEIGHT; i++){
        new_node->next[i] = NULL;
    }

    for (int i = levels; i >= 0; i--){      /* For each level */

        /* Do not search from the beginning of the next level  => Search from the previous node on the level above */
        if (prev != NULL)
            current = prev;
        else
            current = this->listHead->next[i];

        flag = 0;
        prev = NULL;

        /* If there is only the info node, create the first node => then continue to the next level */
        if (this->listHead->next[i] == NULL){
            this->listHead->next[i] = new_node;
            continue;  
        }

        /* Insert as the first SkipNode => then continue to the next level */
        if (id < current->citizenID){

            new_node->next[i] = this->listHead->next[i];
            this->listHead->next[i] = new_node;
            continue;
        }

        while (id >= current->citizenID){

            if (current->next[i] == NULL){

                /* Insert as the last SkipNode */
                current->next[i] = new_node;
                flag = 1;
                break;

            }else{
                prev = current;
                current = current->next[i];
            }
        }
        
        /* Skip node already inserted at this level => continue to the next level */
        if (flag)
            continue;

        prev->next[i] = new_node;
        new_node->next[i] = current;
    }

    return new_node;
}

int SkipList::SkipListSearch(int key, CitizenNode** citNode){

    SkipNode* current;
    SkipNode* prev = NULL;

    for (int lvl = (MAX_HEIGHT - 1); lvl >= 0; lvl--){      /* For every level from the max_level until level 0 */

        /* Do not search from the beginning of the next level  => Search from the previous node on the level above */
        if (prev != NULL)
            current = prev;
        else
            current = this->listHead->next[lvl];

        prev = NULL;

        while ((current != NULL) && (key >= current->citizenID)){       /* Search until the end of the current level or until key < of the current citizenId */

            if (key == current->citizenID){     /* Found the element we are looking for => Return 1 */
                *citNode = current->citNode;
                return 1;
            }

            prev = current;
            current = current->next[lvl];         /* Go to the next SkipNode in level i */
        }
    }
    
    *citNode = NULL;
    return 0;           /* Return 0 => The element with id = key doesn't exist in this Skip List */
}

int SkipList::SkipListRemove(int key){

    SkipNode* current = NULL;
    SkipNode* prev = NULL;
    SkipNode* temp = NULL;              /* The SkipNode that will be deleted */

    for (int i = (MAX_HEIGHT - 1); i >= 0; i--){

        /* Do not search from the beginning of the next level  => Search from the previous node on the level above */
        if (prev != NULL)
            current = prev;
        else
            current = this->listHead->next[i];

        prev = NULL;

        while ((current != NULL) && (key >= current->citizenID)){       /* Search until the end of the current level or until key < of the current citizenId */ 

            if (key == current->citizenID){             /* Found the element we want to remove */

                if (prev != NULL)
                    prev->next[i] = current->next[i];            /* Connect the previous node with the next one of the one that we are deleting */
                else
                    listHead->next[i] = current->next[i];        /* If we delete the first node in this level => the previous node is the list head->next[i] */
                
                temp = current;
                break;
            }
            prev = current;
            current = current->next[i];
        }
    }

    if (temp != NULL){
        delete temp;
        cout << "Citizen with id: " << key << " deleted successfully from Skip List." << endl; 
        return 1;
    }else{
        cout << "There is no citizen with id: " << key << ". Nothing got deleted. " << endl;
        return 0;
    }
}


int SkipList::getRandomLevel(){

    int level = 0;
    while (CoinFlip() && (level < MAX_HEIGHT - 1)){
        level++;
    }
    return level;
}

void SkipList::SkipLevelPrint(int level){

    SkipNode * current = this->listHead->next[level];      // index in array

    while (current != NULL) {
       
        cout << current->citizenID << " ";
        cout << current->citNode->citizen.firstName << " " << current->citNode->citizen.lastName;
        cout << " " << *(current->citNode->citizen.country);
        cout << " " << current->citNode->citizen.age << endl;  
        current = current->next[level];
        
    }
    cout << endl;
}

int SkipList::SkipListCount(int level){

    int counter = 0;
    SkipNode * current = this->listHead->next[level];      // index in array

    cout << endl << "Skip List Level: " << level << endl;
    while (current != NULL) {
        counter++;
        current = current->next[level];
    }
    cout << "The number of elements in list with level " << level << " is: " << counter << endl;
    return counter;
}

/* Get a 50 - 50 chance (heads or tails) */
int SkipList::CoinFlip(){

    if (rand() > RAND_MAX/2)
        return 1;
    else 
        return 0;
}

void SkipList::SkipPopulationStatus(Date date1, Date date2,int CountriesNum, VirusNode* vn, CitizenNode* head){

    SkipNode * current = this->listHead->next[0];
    Date vacDate;

    string Countries[CountriesNum];
    int count[CountriesNum];

    for (int j = 0; j < CountriesNum; j++)      /* Initialize with 0 */
        count[j] = 0;
    
    int i = 0;
    bool flag;

    while (current != NULL){

        flag = 0;
        vacDate = *(current->citNode->citizen.citizenIDptr->vacInfo.dateVaccinated);       /* Get vaccination date */

        /* Check if vaccination Date is between date1 and date2 */
        if (((vacDate.compareDates(date1)) >= 0) && ((vacDate.compareDates(date2)) <= 0)){

            string country = *(current->citNode->citizen.country);
            
            for (int j = 0; j < i; j++){            

                if (country.compare(Countries[j]) == 0){        /* If country is already in the list => update counter */
                    flag = 1;
                    count[j]++;
                    break;
                }
            }

            if (flag == 0){                 /* Insert the new country => go to the next available position in array */
                Countries[i] = country;
                count[i]++;
                i++;
            }
        }
        current = current->next[0];
    }
        
    float CoutryCitizens;

    for(int j = 0; j < i; j++){      /* Print Data */

        CoutryCitizens = CitizenListCount(head,Countries[j]);
        cout << Countries[j] << " " << count[j] << " " << (count[j]/(CoutryCitizens)) * 100 << "%" << endl;
    }
}


void SkipList::SkipPopulationStatus(Date date1, Date date2, VirusNode* vn, string country, CitizenNode* head){

    SkipNode * current = this->listHead->next[0];
    Date vacDate;

    int count = 0;

    while (current != NULL){

        vacDate = *(current->citNode->citizen.citizenIDptr->vacInfo.dateVaccinated);       /* Get vaccination date */

        /* Check if vaccination Date is between date1 and date2 */
        if (((vacDate.compareDates(date1)) >= 0) && ((vacDate.compareDates(date2)) <= 0)){
            
            if (country.compare(*(current->citNode->citizen.country)) == 0){        /* If country is already in the list => update counter */
                count++;
            }  
        }
        current = current->next[0];
    }

    float CoutryCitizens = CitizenListCount(head,country);
    cout << country << " " << count << " " << (count/CoutryCitizens) * 100 << "%" << endl;
}

void SkipList::SkipPopStatusByAge(Date date1, Date date2, int CountriesNum, VirusNode* vn, CitizenNode* head){

    SkipNode * current = this->listHead->next[0];
    Date vaccinationDate;
    string Countries[CountriesNum];       /* */
    int ageGroup[CountriesNum][4];      /* 4 age groups: [0-20] , [20-40] , [40-60] , [60+] */

    for (int j = 0; j < CountriesNum; j++)       /* Initialize every age group for every country with 0 */
        for (int k = 0; k < 4; k++)
            ageGroup[j][k] = 0;
    
    int age,i = 0;
    bool flag;

    while (current != NULL){

        flag = 0;
        vaccinationDate = *(current->citNode->citizen.citizenIDptr->vacInfo.dateVaccinated);       /* Get vaccination date */

        /* Check if vaccination Date is between date1 and date2 */
        if (((vaccinationDate.compareDates(date1)) >= 0) && ((vaccinationDate.compareDates(date2)) <= 0)){

            string country = *(current->citNode->citizen.country);
            
            for (int j = 0; j < i; j++){            

                if (country.compare(Countries[j]) == 0){        /* If country is already in the list => update counter */
                    flag = 1;
                    age = current->citNode->citizen.age;

                    if ((age >= 0) && (age <= 20))
                        ageGroup[j][0]++;
                    else if ((age >= 20) && (age <= 40))
                        ageGroup[j][1]++;
                    else if ((age >= 40) && (age <= 60))
                        ageGroup[j][2]++;
                    else
                        ageGroup[j][3]++;     
                    break;
                }
            }

            if (flag == 0){                 /* Insert the new country => go to the next available position in array */
                Countries[i] = country;
                
                age = current->citNode->citizen.age;

                if ((age >= 0) && (age <= 20))
                    ageGroup[i][0]++;
                else if ((age >= 20) && (age <= 40))
                    ageGroup[i][1]++;
                else if ((age >= 40) && (age <= 60))
                    ageGroup[i][2]++;
                else
                    ageGroup[i][3]++;
                
                i++;
            }
        }
        current = current->next[0];
    }
    
    float allAgeGroup[4];

    for (int j = 0; j < i; j++){              /* For every Country */

        CitizenListCountByAge(head, Countries[j] , allAgeGroup);

        /* Print Data */
        cout << Countries[j] << endl;
        cout << "0-20 " << ageGroup[j][0] << " " << (ageGroup[j][0]/allAgeGroup[0]) * 100 << "% " << endl;
        cout << "20-40 " << ageGroup[j][1] << " " << (ageGroup[j][1]/allAgeGroup[1]) * 100 << "% " << endl;
        cout << "40-60 " << ageGroup[j][2] << " " << (ageGroup[j][2]/allAgeGroup[2]) * 100 << "% " << endl;
        cout << "60+ " << ageGroup[j][3] << " " << (ageGroup[j][3]/allAgeGroup[3]) * 100 << "% " << endl << endl;
    }
}

void SkipList::SkipPopStatusByAge(Date date1, Date date2, VirusNode* vn, string country, CitizenNode* head){

    SkipNode * current = this->listHead->next[0];
    Date vacDate;

    int count = 0;
    int ageGroup[4] = {0,0,0,0};

    int age;

    while (current != NULL){

        vacDate = *(current->citNode->citizen.citizenIDptr->vacInfo.dateVaccinated);       /* Get vaccination date */

        /* Check if vaccination Date is between date1 and date2 */
        if (((vacDate.compareDates(date1)) >= 0) && ((vacDate.compareDates(date2)) <= 0)){
            
            if (country.compare(*(current->citNode->citizen.country)) == 0){        /* If country is already in the list => update counter */
                count++;

                age = current->citNode->citizen.age;

                if ((age >= 0) && (age <= 20))
                    ageGroup[0]++;
                else if ((age >= 20) && (age <= 40))
                    ageGroup[1]++;
                else if ((age >= 40) && (age <= 60))
                    ageGroup[2]++;
                else
                    ageGroup[3]++;
            }  
        }
        current = current->next[0];
    }

    float allAgeGroup[4];
    CitizenListCountByAge(head, country , allAgeGroup);     /* Get all country's citizens seperated in age groups */

    cout << country << endl;
    cout << "0-20 " << ageGroup[0] << " " << (ageGroup[0]/allAgeGroup[0]) * 100 << "%" << endl;
    cout << "20-40 " << ageGroup[1] << " " << (ageGroup[1]/allAgeGroup[1]) * 100 << "%" << endl;
    cout << "40-60 " << ageGroup[2] << " " << (ageGroup[2]/allAgeGroup[2]) * 100 << "%" << endl;
    cout << "60+ " << ageGroup[3] << " " << (ageGroup[3]/allAgeGroup[3]) * 100 << "%" << endl << endl;
}