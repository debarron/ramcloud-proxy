#include "RCEntry.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <string.h>

using namespace std;

RCEntry::RCEntry(string key, const char* bytes, uint32_t bytesLength){
  this->key = key;
  this->bytesLength = bytesLength;
  this->bytes = new char[bytesLength];
  memcpy(this->bytes, bytes, bytesLength);
}

RCEntry::~RCEntry(){
  delete []bytes;
}

