#ifndef READ_FILE_H_
#define READ_FILE_H_

/* Include my lists from project1*/
#include "CitizenList.h"
#include "CountryList.h"
#include "VirusList.h"

int readFile(string fileName, int bloomSize, CitizenNode** CitizenListHead, VirusNode** VirusListHead, CountryNode** CountryListHead);


#endif