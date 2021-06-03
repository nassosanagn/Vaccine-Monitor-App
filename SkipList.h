#ifndef SKIP_LIST_H_
#define SKIP_LIST_H_

#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>

#include "Date.h"
#include "CitizenList.h"

#define MAX_HEIGHT 10

typedef struct SkipNode{
    int citizenID;
    struct CitizenNode* citNode;
    struct SkipNode** next;   
}SkipNode;

class SkipList{
    private:
        struct SkipNode* listHead;
        int maxHeight;                  /* max depth of the skip list */

    public:

        /* Accessors */
        int getMaxHeight() const{ return maxHeight;}
        SkipNode* getSkipListHead() const{ return listHead;}

        SkipList(int maxLevel);             /* Skip List Constructor */
        ~SkipList();                    /* Skip List Destructor */

        /* Insert an element in the Skip List */
        SkipNode* SkipListInsert(int id, CitizenNode* head);

        /* Return 1 if the element is in the list or 0 if it's not */
        int SkipListSearch(int key, CitizenNode** cit = NULL);
        
        /* Delete the node with citizenId == "key", if it exists in the skip list */
        int SkipListRemove(int key);

        /* Returns a random level to insert the element from index [0 - (MAX_HEIGHT - 1)] */
        int getRandomLevel();

        /* Print all the nodes on a specific level in the skip list */
        void SkipLevelPrint(int level = 0);

        /* Count the elements of a specific level in the skip list */
        int SkipListCount(int level = 0);

        /* Get a 50 - 50 chanche (heads or tails) */
        int CoinFlip();

        /* Used for command 4 */
        void SkipPopulationStatus(Date date1, Date date2, int countriesNum, struct VirusNode* vn, CitizenNode* head);      /* if there is no argument country given */
        void SkipPopulationStatus(Date date1, Date date2, struct VirusNode* vn, string country, CitizenNode* head);  /* With argument country given */

        /* Used for command 5 */
        void SkipPopStatusByAge(Date date1, Date date2, int countriesNum, struct VirusNode* vn, CitizenNode* head);       /* if there is no argument country given */
        void SkipPopStatusByAge(Date date1, Date date2, struct VirusNode* vn, string country, CitizenNode* head);        /* With argument country given */
};

#endif /* SKIP_LIST_H_ */