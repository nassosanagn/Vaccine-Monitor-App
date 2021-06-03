#include "Date.h"
using namespace std;

Date::Date(int d, int m, int y){
    this->day = d;
    this->month = m;
    this->year = y;
}

/* Setters */
void Date::setDay(int d){ day = d;}
void Date::setMonth(int m){ month = m;}
void Date::setYear(int y){ year = y;}

/* Accessors */
int Date::getDay() const{return day;}
int Date::getMonth() const{return month;}
int Date::getYear() const{return year;}

/* If date1 > date2 return a number > 0 
If date1 == date2 return a number == 0
If date 1 < date 2 return a number < 0 */

int Date::compareDates(Date date2){

    /* Compare Years */
    if (this->getYear() < date2.getYear()){
        return -1;

    }else if(this->getYear() > date2.getYear()){
        return 1;
    
    }else{          /* Same year => compare months */

        if (this->getMonth() < date2.getMonth()){
            return -1;

        }else if(this->getMonth() > date2.getMonth()){
            return 1;
        
        }else{      /* Same month => compare days */

            if (this->getDay() < date2.getDay()){
                return -1;

            }else if(this->getDay() > date2.getDay()){
                return 1;

            }else{
                return 0;
            }
        }
    }
}

int Date::getDateFromStr(string str){

    char* temp = const_cast<char*>(str.c_str());
    this->day = stoi(strtok(temp,"-"));
    this->month = stoi(strtok(NULL,"-"));
    this->year = stoi(strtok(NULL,"- \n"));
    return 1;
}

int Date::getTodaysDate(){

    time_t currentTime = time(NULL);
    tm *lTime = localtime(&currentTime);
      
    this->day = lTime->tm_mday;
    this-> month = 1 + lTime->tm_mon;
    this->year = 1900 + lTime->tm_year;
    return 1;
}

void Date::printDate(){
    cout << this->day << "-" << this->month << "-" << this->year;
}

string Date::convertDateToStr(){
      
    string day = to_string(this->day);
    string month = to_string(this->month);
    string year = to_string(this->year);

    return day + "-" + month + "-" + year;
}

bool Date::lessThan6Months(Date travelDate){
      
    int year = travelDate.getYear() - this->year;

    if (year == 0){   /* It's the same year */

        int month = travelDate.getMonth() - this->month;

        if (month == 6){

            int day = travelDate.getDay() - this->day;

            if (day < 0)
                return true;

            else  
                return false;

        }else if (month < 6)
            return true;
        else
            return false;

    }else if (year == 1){

    int month = this->month;

        for(int i = 0; i < 6; i++){
            if (month == 12)
            month = 0;
            month++;
        }

        if (travelDate.getMonth() < month)
            return true;
        else if (travelDate.getMonth() > month)
            return false;
        else{
            if((this->day - travelDate.getDay()) > 0)
                return true;
            else
                return false;
        }

    }else
    return false;
}