#include "RCProxy.h"

#include <iostream>

using namespace std;
using namespace RAMCloud;

RCProxy::RCProxy(char *serviceLocator, char *clusterName){
  clock_t start;
  
  memset(this->info, 0, INFO_LENGTH);
  this->options.coordinatorLocator = serviceLocator;
  this->options.clusterName = clusterName;

  start = clock();
  this->client = new RamCloud(&this->options);
  _log(start);
}

//RCProxy::~RCProxy(){
  // Stop the client
//}

void RCProxy::_log(clock_t since){
  this->elapsedSecsLog = double(clock() - since)/CLOCKS_PER_SEC;
}

bool RCProxy::_isMultiReadRequestOK(MultiReadObject *request){
  bool isOK = (request->status == STATUS_OK);
  if(!isOK) this->info[I_ERRORS]++;
  return isOK;
}

bool RCProxy::_isObjectBufferNULL(Tub<ObjectBuffer> *buffer){
  bool isNULL = (buffer->get() == NULL);
  if(isNULL) this->info[I_NULLS]++;
  return isNULL;
}

RCTable* RCProxy::getTable(char *tableName){
  uint64_t tableId;
  clock_t start;

  try{
    start = clock();
    tableId = this->client->getTableId(tableName);
  }
  catch(TableDoesntExistException &e){
    tableId = -1;
  }

  _log(start);
  return new RCTable(tableName, tableId);
}

RCTable* RCProxy::createTable(char *tableName, int serverSpan){
  clock_t start;
  RCTable *table;
 
  start = clock();
  table = getTable(tableName);
  if (table->tableId == -1){
    table->tableId = this->client->createTable(tableName, serverSpan);
  }

  _log(start);
  return table;
}

void RCProxy::_cleanInfo(){
  memset(info, 0, INFO_LENGTH);
}

uint32_t* RCProxy::getInfoPtr(){
  return info;
}

vector<RCEntry>* RCProxy::_getMultipleEntries(MultiReadObject **requests, 
Tub<ObjectBuffer> *buffers){

  uint32_t dataLength, keyLength;
  ObjectBuffer *result;
  vector<RCEntry> *entries;
  char *data;
 
  _cleanInfo();
  entries = new vector<RCEntry>();
  for(uint32_t i = 0; i < info[I_REQUESTS]; i++){
    if(!_isMultiReadRequestOK(requests[i])) continue;
    else if(_isObjectBufferNULL(&buffers[i])) continue;

    result = buffers[i].get();
    string key = reinterpret_cast<const char *>(result->getKey(0));
    data = (char *)result->getValue(&dataLength);
    keyLength = result->getKeyLength(0);

    info[I_BYTES] = info[I_BYTES] + dataLength;
    entries->push_back(RCEntry(key, data, dataLength));
  }

  return entries;
}

RCRelation* RCProxy::_multiPull(RCTable *table, vector<string> &keys){
  clock_t start;
  int keysLength = keys.size();
  vector<RCEntry> *entries;

  //Prepare multiread
  Tub<ObjectBuffer> buffers[keysLength];
  MultiReadObject *requests[keysLength];
  MultiReadObject requestedObjects[keysLength];
  for (uint32_t i = 0; i < keysLength; i++){
    string key = keys[i];
    requestedObjects[i] = MultiReadObject(
        table->tableId,
        key.data(), 
        key.length(), 
        &buffers[i]
    );
    requests[i] = &requestedObjects[i];
  }

  // Perform mutiread
  start = clock();
  this->client->multiRead(requests, keysLength);
  _log(start);


  uint32_t dataLength, keyLength;
  ObjectBuffer *result;
  char *data;
 
  _cleanInfo();
  entries = new vector<RCEntry>();
  for(uint32_t i = 0; i < keysLength; i++){
    if(!_isMultiReadRequestOK(requests[i])) continue;
    else if(_isObjectBufferNULL(&buffers[i])) continue;

    result = buffers[i].get();
    string key = reinterpret_cast<const char *>(result->getKey(0));
    data = (char *)result->getValue(&dataLength);
    keyLength = result->getKeyLength(0);

    info[I_BYTES] = info[I_BYTES] + dataLength;
    entries->push_back(RCEntry(key, data, dataLength));
  }

  return new RCRelation(table, entries);
}

RCRelation* RCProxy::_singlePull(RCTable *table, vector<string> &keys){
  clock_t start;
  uint32_t dataLength;
  Buffer buffer;
  char *data;
  vector<RCEntry> *entry;

  _cleanInfo();
  start = clock();
  this->client->read(table->tableId, keys[0].data(), keys[0].length(), &buffer);
  _log(start);

  Buffer::Iterator reader(&buffer);
  data = (char *)reader.getData();
  dataLength = reinterpret_cast<uint32_t>(reader.getLength());

  if(data == NULL) info[I_NULLS]++;
  entry = new vector<RCEntry>();
  entry->push_back(RCEntry(keys[0], data, dataLength));

  return new RCRelation(table, entry);
}

RCRelation* RCProxy::pull(RCTable *table, vector<string> &keys){
  return (keys.size() == 1) ? _singlePull(table, keys) : _multiPull(table, keys);
}

void RCProxy::_singlePush(RCRelation *input){
  clock_t start;
  uint64_t version;
  RCTable *table;

  RCEntry &entry = (*input->entries)[0]; 
  table = input->table;

  _cleanInfo();
  start = clock();
  this->client->write(
      table->tableId, 
      entry.key.data(), 
      entry.key.length(), 
      entry.bytes, 
      entry.bytesLength,
      NULL,
      &version,
      false
  );
  _log(start);
}

void RCProxy::_setMultiWriteRequest(void *requestPointer, RCTable *table, RCEntry &entry){
  new(requestPointer) MultiWriteObject(
      table->tableId,
      entry.key.data(),
      entry.key.length(),
      entry.bytes,
      entry.bytesLength,
      NULL
  );
}

void RCProxy::_multiPush(RCRelation *input){
  clock_t start;
  RCTable *table;
  uint32_t requestCount;
  uint64_t bytesWritten;

  table = input->table;
  vector<RCEntry> &entries = (*input->entries);
  requestCount = entries.size();

  bytesWritten = 0;
  MultiWriteObject requestBuffer[requestCount];
  MultiWriteObject *requestPointer[requestCount];
  for(uint32_t i = 0; i < requestCount; i++){
    RCEntry &entry = entries[i];
    bytesWritten =+ entry.bytesLength;

    requestPointer[i] = &requestBuffer[i];
    new(requestPointer[i]) MultiWriteObject(
      table->tableId,
      entry.key.data(),
      entry.key.length(),
      entry.bytes,
      entry.bytesLength,
      NULL
    );
  }

  _cleanInfo();
  start = clock();
  this->client->multiWrite(requestPointer, requestCount);
  _log(start);
}

void RCProxy::push(RCRelation *input){
  return (input->entries->size() == 1) ? _singlePush(input) : _multiPush(input);
}

void RCProxy::dropTable(const char *tableName){
  this->client->dropTable(tableName);
}


vector<string> RCProxy::enumerateKeysFromTable(RCTable *table){
  vector<string> keys;

  TableEnumerator *enumerator;
  bool isValid = true;
  uint32_t keyLength, dataLength;
  const void *key, *data;

  enumerator = new TableEnumerator(*this->client, table->tableId, true);
  while(enumerator->hasNext() && isValid){
    key = NULL;
    data = NULL;
    keyLength = 0;
    dataLength = 0;

    enumerator->nextKeyAndData(&keyLength, &key, &dataLength, &data);
    
    isValid = (key != NULL);
    if(isValid){
      char *keyStr = new char[keyLength + 1];
      memcpy(keyStr, (char *)key, keyLength);
      keyStr[keyLength] = '\0';

      keys.push_back(string(keyStr));
      cout << " READ key " << keyStr << "\n";
    }
  }

  return keys;
}


 








