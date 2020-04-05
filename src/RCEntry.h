#ifndef RCENTRY_H
#define RCENTRY_H

#include <cstdint>
#include <string>
#include <cstring>

using namespace std;

class RCEntry {
  public:
    string key;
    char *bytes;
    uint32_t bytesLength;
    RCEntry(string, const char*, uint32_t);
//    ~RCEntry();
};

#endif
