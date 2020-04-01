#ifndef RCENTRY_H
#define RCENTRY_H

#include <cstdint>
#include <string>

using namespace std;

class RCEntry {
  public:
    string key;
    char *bytes;
    uint32_t bytesLength;
    RCEntry(string, char*, uint32_t);
//    ~RCEntry();
};

#endif
