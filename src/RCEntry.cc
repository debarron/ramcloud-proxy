#include "RCEntry.h"

RCEntry::RCEntry(string key, char* bytes, uint32_t bytesLength){
  this->key = key;
  this->bytes = bytes;
  this->bytesLength = bytesLength;
}

/*RCEntry::~RCEntry(){
  delete []bytes;
}
*/
