#include <string>
#include <sstream>
#define HOUR 3600
#define MIN 60

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long Totseconds) {
    string HHSSMM; 
    
    // get HH:MM:SS formate
    int hour=(int)(Totseconds/HOUR);
    int second=(int)(Totseconds % HOUR);
    int minute=(int)(second/MIN);
    second = (int)(second % MIN);

    //add leading zero if necessary
    string strHour = (hour < 10) ? "0" + to_string(hour) : to_string(hour);
    string strMin = (minute < 10) ? "0" + to_string(minute) : to_string(minute);
    string strSec = (second < 10) ? "0" + to_string(second) : to_string(second);
 
    HHSSMM = strHour  + ':' + strMin + ':' + strSec;
    
    return HHSSMM;
 }