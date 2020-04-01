#ifndef RCPROXY_H
#define RCPROXY_H

#define I_ERRORS    0
#define I_NULLS     1
#define I_REQUESTS  2
#define I_BYTES     3
#define INFO_LENGTH 4

#include "RCEntry.h"
#include "RCTable.h"
#include "RCRelation.h"

#include "RamCloud.h"
#include "Buffer.h"
#include "Object.h"
#include "ObjectBuffer.h"
#include "ClientException.h"
#include "MultiRead.h"
#include "Tub.h"

#include <ctime>
#include <sys/types.h>
#include <stdio.h>
#include <vector>
#include <string>

using namespace std;
using namespace RAMCloud;

class RCProxy{
  private:
    uint32_t info[INFO_LENGTH];
    void _log(clock_t);
    bool _isMultiReadRequestOK(MultiReadObject*);
    bool _isObjectBufferNULL(Tub<ObjectBuffer>*);
    void _cleanInfo();
    vector<RCEntry>* _getMultipleEntries(MultiReadObject**, Tub<ObjectBuffer>*);
    RCRelation* _multiPull(RCTable*, vector<string>&);
    RCRelation* _singlePull(RCTable*, vector<string>&);
    void _singlePush(RCRelation*);
    void _multiPush(RCRelation*); 
    void _setMultiWriteRequest(void*, RCTable*, RCEntry&);

  public:
    double elapsedSecsLog;
    RamCloud *client;
    CommandLineOptions options;
    RCProxy(char *, char *);
    //~RCProxy();
    RCTable* getTable(char *);
    RCTable* createTable(char *, int);
    uint32_t* getInfoPtr();
    RCRelation* pull(RCTable *, vector<string>&);
    void push(RCRelation*);
    void dropTable(const char *tableName);
};

#endif
