#ifndef RCTABLE_H
#define RCTABLE_H

#include <cstdint>
#include <iostream>
#include <string>

using namespace std;

class RCTable{
  public:
    uint64_t tableId;
    string tableName;
    RCTable(string&, uint64_t&);
};

#endif
