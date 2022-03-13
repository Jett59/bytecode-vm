#ifndef _HELPER_H
#define _HELPER_H

#include <string>

static inline bool endsWith(const std::string &s, const std::string &suffix) {
  if (s.length() >= suffix.length()) {
    return s.compare(s.length() - suffix.length(), suffix.length(), suffix) ==
           0;
  } else {
    return false;
  }
}

#endif