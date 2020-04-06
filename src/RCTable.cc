#include "RCTable.h"

RCTable::RCTable(char* tableName, uint64_t tableId){
  this->tableName = tableName;
  this->tableId = tableId;
}

RCTable::~RCTable(){
}
