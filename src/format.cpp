#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
    long int hours = seconds / 3600;
    long int minutes = (seconds - hours*3600)/60;
    long int secs = seconds - hours*3600 - minutes*60;
    return std::to_string(hours) + ":" + std::to_string(minutes) + ":" + std::to_string(secs);
}