#ifndef RCRELATION_H
#define RCRELATION_H

#include <cstdint>
#include <vector>
#include <iostream>
#include "RCTable.h"
#include "RCEntry.h"

using namespace std;

class RCRelation{
  public:
    RCTable* table;
    vector<RCEntry> *entries;
    RCRelation(RCTable*, vector<RCEntry>*);
    void cleanEntries();
    ~RCRelation();
};

#endif
