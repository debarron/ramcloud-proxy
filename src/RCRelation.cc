#include "RCRelation.h"

#include <vector>
#include <iostream>
#include "RCTable.h"
#include "RCEntry.h"
#include <cstdint>

using namespace std;

RCRelation::RCRelation(RCTable* table, vector<RCEntry> *entries){
  this->table = table;
  this->entries = entries;
}

RCRelation::~RCRelation(){
  cleanEntries();
}

void RCRelation::cleanEntries(){
  if(this->entries->size() > 0)
    this->entries->clear();
}
