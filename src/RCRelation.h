#ifndef RCRELATION_H
#define RCRELATION_H

#include "RCEntry.h"
#include "RCTable.h"
#include <vector>

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
