#ifndef RCRELATION_H
#define RCRELATION_H
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
