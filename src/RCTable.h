#ifndef RCTABLE_H
#define RCTABLE_H

#include <iostream>

#ifdef __cplusplus
#include <cstdint>
#include <string>
#else
#include <stdint.h>
#include <string.h>
#endif

using namespace std;

class RCTable{
  public:
    uint64_t tableId;
    string tableName;
    RCTable(string, uint64_t);
};

#endif
