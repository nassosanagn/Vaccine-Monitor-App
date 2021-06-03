#ifndef REQUEST_LIST_H_
#define REQUEST_LIST_H_

#include "Date.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

#include <stdio.h>
#include <fstream>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

using namespace std;

typedef struct RequestNode{
   Date requestsDate;
   string country;
   int requestsCounter = 0;
   struct RequestNode* next;
}RequestNode;

void RequestListPush(RequestNode ** head, Date requestsDate, string countryName);

int RequestListCount(RequestNode* head);

int RequestListCount(RequestNode * head, Date date1, Date date2);                   /* count the dates between date1 and date2 */
int RequestListCount(RequestNode * head, string countryName, Date date1, Date date2);

/* Delete all nodes in the list */
void RequestListDelete(RequestNode** head);


#endif /* COUNTRY_LIST_H_ */