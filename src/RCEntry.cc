#include "RCEntry.h"

RCEntry::RCEntry(string key, const char* bytes, uint32_t bytesLength){
  this->key = key;
  this->bytes = new char[bytesLength];
  memcpy(this->bytes, bytes, bytesLength);
  this->bytesLength = bytesLength;
}

RCEntry::~RCEntry(){
  delete bytes;
}

