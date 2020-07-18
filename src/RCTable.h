#ifndef RCTABLE_H
#define RCTABLE_H

#include <iostream>
#include <string>

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

using namespace std;

class RCTable{
  public:
    uint64_t tableId;
    string tableName;
    RCTable(string, uint64_t);
};

#endif
