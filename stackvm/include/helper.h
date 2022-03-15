#ifndef _HELPER_H
#define _HELPER_H

#include <cstddef>
#include <fstream>
#include <string>

static inline bool startsWith(const std::string &s, const std::string &prefix) {
  return s.rfind(prefix, 0) == 0;
}
static inline bool endsWith(const std::string &s, const std::string &suffix) {
  if (s.length() >= suffix.length()) {
    return s.compare(s.length() - suffix.length(), suffix.length(), suffix) ==
           0;
  } else {
    return false;
  }
}
static size_t getFileSize(std::istream &input) {
  input.seekg(0, std::ios::end);
  size_t length = input.tellg();
  input.seekg(0, std::ios::beg);
  return length;
}

#endif