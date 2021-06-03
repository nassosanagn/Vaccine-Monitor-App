#ifndef DATE_H_
#define DATE_H_

#include <iostream>
#include <ctime>
#include <string.h>
#include <cstring>

class Date{
  private:
    int day;
    int month;
    int year;

  public:

    /* Constructor */
    Date(int d = 1, int m = 1, int y = 2000);
    
    /* Setters */
    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);
     
    /* Accessors */
    int getDay() const;
    int getMonth() const;
    int getYear() const;

    /* Compare 2 dates to see whitch is older */
    int compareDates(Date date2);

    /* Get today's date e.g. 27-12-2020 */
    int getTodaysDate();

    /* Print date e.g. 27-12-2020 */
    void printDate();

    /* Get date from string e.g. 27-12-2020 */
    int getDateFromStr(std::string str);

    bool lessThan6Months(Date travelDate);

    std::string convertDateToStr();

};

#endif