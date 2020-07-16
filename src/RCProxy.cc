#include "RCProxy.h"


using namespace std;
using namespace RAMCloud;

void RCProxy::_log(clock_t since){
  this->elapsedSecsLog = double(clock() - since)/CLOCKS_PER_SEC;
}

void RCProxy::_cleanInfo(){
  info[I_ERRORS] = 0;
  info[I_NULLS] = 0;
  info[I_REQUESTS] = 0;
  info[I_BYTES] = 0;
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

void RCProxy::_deletePointer(void *p){
  delete p;
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

void RCProxy::_setMultiWriteRequest(
void *requestPointer, 
RCTable *table, 
RCEntry &entry){

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
    _setMultiWriteRequest(requestPointer[i], table, entry);
    /*
    new(requestPointer[i]) MultiWriteObject(
      table->tableId,
      entry.key.data(),
      entry.key.length(),
      entry.bytes,
      entry.bytesLength,
      NULL
    );*/
  }

  _cleanInfo();
  start = clock();
  this->client->multiWrite(requestPointer, requestCount);
  _log(start);
}

RCRelation* RCProxy::_singlePull(RCTable *table, vector<string> &keys){
  clock_t start;
  Buffer buffer;
  vector<RCEntry> *entry;

  _cleanInfo();
  start = clock();
  this->client->read(table->tableId, keys[0].data(), keys[0].length(), &buffer);
  _log(start);

  Buffer::Iterator reader(&buffer);
  const char *data = reinterpret_cast<const char *>(reader.getData());
  uint32_t dataLength = reinterpret_cast<uint32_t>(reader.getLength());

  if(data == NULL) {
    info[I_NULLS]++;
    return NULL;
  }

  entry = new vector<RCEntry>();
  entry->push_back(RCEntry(keys[0], data, dataLength));
  return new RCRelation(table, entry);
}

void RCProxy::_setMultiReadRequest(
void *requestPointer, 
RCTable *table, 
string key,
Tub<ObjectBuffer> *buffer){

  const char *keyValue = key.data();
  new(requestPointer) MultiReadObject(
      table->tableId,
      keyValue,
      key.length(),
      buffer
  );
}

void RCProxy::_readEntries(
MultiReadObject **requests, 
Tub<ObjectBuffer> *buffers,
uint32_t requestCount,
vector<RCEntry> &entries){

  uint32_t dataLength, keyLength;
  ObjectBuffer *result;
 
  _cleanInfo();
  entries.clear();
  info[I_REQUESTS] = requestCount;
  for(uint32_t i = 0; i < requestCount; i++){
    if(!_isMultiReadRequestOK(requests[i])) continue;
    else if(_isObjectBufferNULL(&buffers[i])) continue;

    result = buffers[i].get();
    const char *key = reinterpret_cast<const char *>(result->getKey(0));
    const char *data = reinterpret_cast<const char *>(result->getValue(&dataLength));

    string key_(key);
    entries.push_back(RCEntry(key_, data, dataLength));
    info[I_BYTES] += dataLength;
  }
}


RCRelation* RCProxy::_multiPull(RCTable *table, vector<string> &keys){

  clock_t start;
  uint32_t keysCount = keys.size();
  vector<RCEntry> *entries;
  
  Tub<ObjectBuffer> buffers[keysCount];
  MultiReadObject *requests[keysCount];
  MultiReadObject requestedObjects[keysCount];
  for (uint32_t i = 0; i < keysCount; i++){
    _setMultiReadRequest(&requestedObjects[i], table, keys[i], &buffers[i]);
    requests[i] = &requestedObjects[i];

    //const char *key = keys[i].data();
    //requestedObjects[i] = MultiReadObject(table->tableId, key, keys[i].length(), &buffers[i]);
    //requests[i] = &requestedObjects[i];
  }

  start = clock();
  this->client->multiRead(requests, keysCount);
  _log(start);
 
  entries = new vector<RCEntry>();
  _readEntries(requests, buffers, keysCount, *entries);
  RCRelation *result = new RCRelation(table, entries);

  //for(uint32_t i = 0; i < keysCount; i++)
  //  _deletePointer(requests[i]);

  return result;
  /*ObjectBuffer *result;
  uint32_t dataLength;

  entries = new vector<RCEntry>();
  for(uint32_t i = 0; i < keysCount; i++){
    if(!_isMultiReadRequestOK(requests[i])) continue;
    else if(_isObjectBufferNULL(&buffers[i])) continue;

    ObjectBuffer *result = buffers[i].get();
    uint32_t dataLength;

    const char *key = reinterpret_cast<const char *>(result->getKey(0));
    const char *data = reinterpret_cast<const char *>(result->getValue(&dataLength));
    entries->push_back(RCEntry(string(key), dataKept, dataLength));

    info[I_BYTES] = info[I_BYTES] + dataLength;
  }*/

}


RCProxy::RCProxy(char *serviceLocator, char *clusterName){
  clock_t start;
  
  memset(this->info, 0, INFO_LENGTH);
  this->options.coordinatorLocator = serviceLocator;
  this->options.clusterName = clusterName;

  start = clock();
  this->client = new RamCloud(&this->options);
  _log(start);
}

RCProxy::~RCProxy(){
//  delete client;
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
  string tableName_(tableName);
  return new RCTable(tableName_, tableId);
}

RCTable* RCProxy::createTable(char *tableName, int serverSpan){
  clock_t start;
  RCTable *table;
 
  start = clock();
  table = getTable(tableName);
  if (table->tableId == -1)
    table->tableId = this->client->createTable(tableName, serverSpan);

  _log(start);
  return table;
}

uint32_t* RCProxy::getInfoPtr(){
  return info;
}

RCRelation* RCProxy::pull(RCTable *table, vector<string> &keys){
  return (keys.size() == 1) ? _singlePull(table, keys) : _multiPull(table, keys);
}

void RCProxy::push(RCRelation *input){
  return (input->entries->size() == 1) ? _singlePush(input) : _multiPush(input);
}

void RCProxy::dropTable(const char *tableName){
  this->client->dropTable(tableName);
}

vector<string> *RCProxy::listKeys(RCTable *table){
  vector<string> *keys;
  uint32_t bufferLength, keyLength;
  const void *buffer;

  TableEnumerator enumerator (*this->client, table->tableId, true);
  keys = new vector<string>();
  while(enumerator.hasNext()){
    keyLength = bufferLength = 0;
    enumerator.next(&bufferLength, &buffer);
    Object enumObject (buffer, bufferLength);

    keyLength = enumObject.getKeyLength();
    string key = string(reinterpret_cast<const char *>(enumObject.getKey()), keyLength);
    keys->push_back(key);
  }

  return keys;
}



