#include <string>
#include <math.h>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int sec = int(seconds%60);
    int minutes = int((seconds/60)%60);
    int hours = int(seconds/3600); 
    string time;
    time.append(ZeroCheck(hours));
    time.append(":");
    time.append(ZeroCheck(minutes));
    time.append(":");
    time.append(ZeroCheck(sec));
    return time; 
}

string Format::ZeroCheck(int time) {
    if (time < 10) return ("0" + std::to_string(time));
    else return std::to_string(time);
}

