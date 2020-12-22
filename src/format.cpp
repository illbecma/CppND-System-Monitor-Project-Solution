#include "format.h"

#include <iomanip>
#include <string>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  int hh = seconds / 3600;
  int rest = seconds % 3600;
  int mm = rest / 60;
  int ss = rest % 60;
  std::ostringstream ostr;
  ostr << std::setfill('0') << std::setw(2) << hh << ":" << std::setfill('0')
       << std::setw(2) << mm << ":" << std::setfill('0') << std::setw(2) << ss;
  return ostr.str();
}