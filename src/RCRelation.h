#ifndef RCRELATION_H
#define RCRELATION_H

#include <vector>
#include <iostream>
#include "RCTable.h"
#include "RCEntry.h"

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

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
