#ifndef BLOOM_LIST_H_
#define BLOOM_LIST_H_

#include "BloomFilter.h"
#include "requestsList.h"

typedef struct BloomNode{     /* Every virus has one bloom filter and 2 skip lists */
   string virusName;
   RequestNode* acceptedReq = NULL;
   RequestNode* totalReq    = NULL;
   RequestNode* rejectedReq = NULL;
   struct BloomFilter* bloomFilter;
   struct BloomNode* next;
}BloomNode;

void BloomListPrint(BloomNode * head);

/* Add a new virusNode in the beginning of the list */
void BloomListPush(BloomNode ** head, string virusName, int bloomFilterSize);

/* Add a new virusNode in the beginning of the list */
void BloomListPush(BloomNode ** head, string virusName, char* bloomArray, int bloomFilterSize);

/* Return the address of VirusNode if "virusName" is in Virus List or NULL if it isn't */
//BloomNode* BloomGetNode(BloomNode* head, string virusName);

/* Return "true" if "virusName" is in Virus List or "false" if it isn't */
bool BloomListSearch(BloomNode* head, string virusName);

bool BloomListMergeBlooms(BloomNode* head, string virusName, char* bloomArray);
int BloomSearchIdInBloom(BloomNode* head, string citizenId, string virusName);

/* Return the number of nodes = different viruses in the list */
int BloomListCount(BloomNode* head);

void BloomListAddTotalReq (BloomNode* head,string virusName, Date reqDate, string countryName);
void BloomListAddAcceptReq(BloomNode* head,string virusName, Date reqDate, string countryName);
void BloomListAddRejectReq(BloomNode* head,string virusName, Date reqDate, string countryName);

int BloomListGetTotalRequests(BloomNode* head, string virusName, Date date1, Date date2);
int BloomListGetAcceptRequests(BloomNode* head,string virusName, Date dat1, Date date2);
int BloomListGetRejectRequests(BloomNode* head,string virusName, Date date1, Date date2);

int BloomListGetTotalRequests(BloomNode* head, string virusName, string countryName, Date date1, Date date2);
int BloomListGetAcceptRequests(BloomNode* head,string virusName, string countryName, Date date1, Date date2);
int BloomListGetRejectRequests(BloomNode* head,string virusName, string countryName, Date date1, Date date2);

/* Delete all nodes in the list */
void BloomListDelete(BloomNode** head);

#endif /* VIRUS_LIST_H_ */