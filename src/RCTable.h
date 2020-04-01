#ifndef RCTABLE_H
#define RCTABLE_H

#include <cstdint>

class RCTable{
  public:
    uint64_t tableId;
    char* tableName;
    RCTable(char*, uint64_t);
//    ~RCTable();
};

#endif
