#include "RCTable.h"

#include <iostream>
#include <cstdint>
#include <string>

RCTable::RCTable(string tableName, uint64_t tableId){
  this->tableName = tableName;
  this->tableId = tableId;
}

