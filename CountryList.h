#ifndef COUNTRY_LIST_H_
#define COUNTRY_LIST_H_

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

typedef struct CountryNode{
   string country;
   struct CountryNode* next;
}CountryNode;

void CountryListPrint(CountryNode * head);
void CountryListPrintInFile(CountryNode * head, ofstream& fileName);

void CountryListPush(CountryNode ** head, string country);

/* Return "true" if "country" is in Country List or "false" if it isn't */
bool CountryListSearch(CountryNode* head, string countryName);

/* Return the address of the string if "countryName" is in Country List or NULL if it isn't */
string* CountryListSearch2(CountryNode* head, string countryName);

/* Return the number of nodes = different countries in the list */
int CountryListCount(CountryNode* head);

/* Delete all nodes in the list */
void CountryDeleteList(CountryNode** head);

string getAllCountries(CountryNode* head);


#endif /* COUNTRY_LIST_H_ */