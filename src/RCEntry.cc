#include "RCEntry.h"

RCEntry::RCEntry(string &key, const char* bytes, uint32_t &bytesLength){
  this->key = key;
  this->bytesLength = bytesLength;
  this->bytes = new char[bytesLength];
  memcpy(this->bytes, bytes, bytesLength);
}

RCEntry::~RCEntry(){
  if (bytes != NULL)
    delete []bytes;
}

