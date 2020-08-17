#ifndef RCTABLE_H
#define RCTABLE_H

#include <string>
#include <cstdint>

using namespace std;

class RCTable{
  public:
    uint64_t tableId;
    string tableName;
    RCTable(string, uint64_t);
};

#endif
