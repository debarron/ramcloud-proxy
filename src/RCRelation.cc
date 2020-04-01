#include "RCRelation.h"

using namespace std;

RCRelation::RCRelation(RCTable* table, vector<RCEntry> *entries){
  this->table = table;
  this->entries = entries;
}

/*
RCRelation::~RCRelation(){
  delete table;
  entries->clear();
}
*/

void RCRelation::cleanEntries(){
  if(this->entries->size() > 0)
    this->entries->clear();
}
