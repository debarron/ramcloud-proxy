#ifndef RCTABLE_H
#define RCTABLE_H

#include <cstdint>
#include <iostream>

using namespace std;

class RCTable{
  public:
    uint64_t tableId;
    char* tableName;
    RCTable(const char*, uint64_t);
    ~RCTable();
};

#endif
