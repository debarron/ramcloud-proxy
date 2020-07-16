#ifndef RCENTRY_H
#define RCENTRY_H

#include <cstdint>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;


/** RCEntry
 * Data passed as argument in the constructor gets
 * copied to another data space, allocated at that
 * time. 
*/ 
class RCEntry {
  public:
    string key;
    char *bytes = NULL;
    uint32_t bytesLength;

    RCEntry(string, const char*, uint32_t);
    ~RCEntry();
};

#endif
