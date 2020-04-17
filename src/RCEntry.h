#ifndef RCENTRY_H
#define RCENTRY_H

#include <cstdint>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;


/** RCEntry
 * When constructed, this class' constructor 
 * copies the content from the const char pointer
 * and allocate memory to copy the content
*/ 
class RCEntry {
  public:
    string key;
    char *bytes = NULL;
    uint32_t bytesLength;
    RCEntry(string&, const char*, uint32_t&);
    ~RCEntry();
};

#endif
