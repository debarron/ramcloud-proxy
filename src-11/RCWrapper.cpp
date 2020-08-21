/**
 * This a re-do of the project using
 * C++ datastructures
*/ 

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <cstdint>
#include <chrono>
#include <string.h>

#include "RamCloud.h"
#include "ClientException.h"
#include "Buffer.h"
#include "ObjectBuffer.h"
#include "MultiRead.h"
#include "Tub.h"

#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;

void RCWrapper::_time_start(){_start = clock();}
void RCWrapper::_time_end(){_end = clock();}
double RCWrapper::elapsed_time(){ return (double)(_end - _start)/CLOCKS_PER_SEC;}
int RCWrapper::_count_entries(Relation &data){
  int total_entries = 0;

  for(RelationIterator it = data.begin(); it != data.end(); it++)
    total_entries += ((vector<Entry>)it->second).size();

  return total_entries;
}

RCWrapper::RCWrapper(string service_locator, string cluster_name){
  if(this->_client == NULL){
    this->_options.coordinatorLocator = service_locator.c_str();
    this->_options.clusterName = cluster_name.c_str();
    this->_client = new RamCloud(&this->_options);
  }
}

// TODO Implement closing the connection to RAMCloud
int RCWrapper::close(){
  return 1;
}

int RCWrapper::count_entries(Relation &data){
  return _count_entries(data);
}

// ****************************
// TABLE OPERATIONS
// ****************************
uint64_t RCWrapper::create_table(string table_name, int server_span){
  return this->_client->createTable(table_name.c_str(), server_span);
}

uint64_t RCWrapper::get_table_id(string table_name){
  uint64_t table_id;

  try{
    table_id = this->_client->getTableId(table_name.c_str());
  }catch(TableDoesntExistException &e){
    table_id = -1;
  }

  return table_id;
}
// ****************************
// TABLE OPERATIONS
// ****************************

void RCWrapper::write(uint64_t table_id, string entry_key, const char *entry_value, uint32_t entry_value_length){
  uint64_t version;

  this->_client->write(
      table_id, entry_key.data(), entry_key.length(),
      entry_value, entry_value_length,
      NULL, &version, false);
}

Entry RCWrapper::read(uint64_t table_id, string key){
  Buffer buffer;
  Entry result;

  this->_client->read(table_id, key.data(), key.length(), &buffer);

  Buffer::Iterator buffer_reader(&buffer);
  const char *const_data = reinterpret_cast<const char*>(buffer_reader.getData());
  uint32_t data_length = reinterpret_cast<uint32_t>(buffer_reader.getLength());
  char *data = (char *) malloc(sizeof(char) * data_length);
  memcpy(data, const_data, data_length);

  result = make_tuple(key, data, data_length);
  return result;
}

void RCWrapper::_multi_read_request(uint64_t table_id, vector<Entry> &data, 
    MultiReadObject *memory_block, Tub<ObjectBuffer> *request_buffer, 
    MultiReadObject **request_pointer, int offset){

  for(vector<Entry>::iterator it = data.begin(); it != data.end(); ++it){
    string key = get<0>(*it);
    const char *key_value = key.data();
    uint32_t key_length = key.length();

    new((void*)&memory_block[offset]) MultiReadObject(table_id, key_value, key_length, request_buffer);
    request_pointer[offset] = &memory_block[offset];
    offset++;
  }
}



Relation *RCWrapper::_multi_read_read_buffer(MultiReadObject *request, Tub<ObjectBuffer> *buffer, int buffer_count){
  ObjectBuffer *buffer_reader;
  uint64_t table_id = request[0].tableId;
  vector<Entry>* entries = new vector<Entry>();
  Relation *result = new Relation();
  uint32_t data_length;

  cout << "** got into the function\n";

  result = new Relation();
  for(int i = 0; i < buffer_count; i++){
    if(table_id != request[i].tableId){
      result->insert(pair<uint64_t, vector<Entry>>(table_id, *entries));
      entries = new vector<Entry>();
      table_id = request[i].tableId;
    }

    cout << "** processing table_id " << table_id << " and entry no. " << i << endl;

    buffer_reader = buffer[i].get();
    cout << "** got the buffer\n";
    const char *buffer_key = reinterpret_cast<const char *>(buffer_reader->getKey(0));
    cout << "** got the key " << buffer_key << "\n";
    int buffer_key_length = buffer_reader->getKeyLength(0);
    cout << "** got the key len " << buffer_key_length << "\n";
    const char *buffer_data = reinterpret_cast<const char *>(buffer_reader->getValue(&data_length));
    cout << "** got the buffer data " << buffer_data << "\n";

    char *data_key = (char *) malloc(sizeof(char) * buffer_key_length);
    memcpy(data_key, buffer_key, buffer_key_length);
    string data_key_str (buffer_key);

    char *data = (char *)malloc(sizeof(char) * data_length);
    memcpy(data, buffer_data, data_length);
    entries->push_back(make_tuple(data_key_str, data, data_length));

    cout << "** got key " << data_key << endl;
  }

  result->insert(pair<uint64_t, vector<Entry>>(table_id, *entries));

  cout << "** inserted in the last table_id " << table_id << endl;

  return result;
}

Relation *RCWrapper::read(Relation &data){
  int total_entries = _count_entries(data);

  MultiReadObject request[total_entries];
  MultiReadObject *request_pointer[total_entries];
  Tub<ObjectBuffer> request_buffer[total_entries];
  int request_index = 0;
  
  for(RelationIterator it = data.begin(); it != data.end(); ++it){
    _multi_read_request((*it).first, (*it).second, &request[request_index], 
        &request_buffer[request_index], &request_pointer[request_index], request_index);
    request_index += ((*it).second).size();
  }

  this->_client->multiRead(request_pointer, total_entries);
  Relation *result = _multi_read_read_buffer(request, request_buffer, total_entries);

  return result;
}

/*
int RCWrapper::_write_relation_from(Relation &data, int start_index, int end_index){
  // Generate the MultiWriteObject from data
  // Write with multiWrite
  
  int multi_write_count = (end_index - start_index) + 1;
  tuple<uint64_t*, Entry*> entries[multi_write_count];
  int entries_index = -1;
  int current_index = 0;
  
  for (RelationIterator it = data.begin(); it != data.end() || current_index > end_index; it++){
    vector<Entry> *entries_pointer = &(*it).second;

    if(start_index > (current_index + entries_pointer->size() - 1)){
      current_index += entries_pointer->size() -1;
      continue;
    }

    int offset = start_index - current_index;
    for(vector<Entry>::iterator entries_iterator = entries_pointer->begin() + offset;
        entries_iterator != entries_pointer->end() && current_index <= end_index; 
        entries_iterator++){

      entries[++entries_index] = make_pair(&(*it).first, &entries_iterator);
      current_index++;
    } 
  }

}

int RCWrapper::write(RCWrapper::Relation &data, int steps = 1){
  int total_entries = _count_entries(data);
  int step_size = total_entries / steps;
  int data_start_index = data_end_index = 0;
  int data_write_count = 0;

  if(steps == 1)
    data_write_count = _write_relation_from(data, data_start_index, total_entries);
  else
    for(int i = 1; i <= steps; i++){
      data_end_index = ((data_start_index + step_size) <= total_entries) ? 
        data_start_index + step_size : total_entries;

      data_write_count += _write_relation_from(data, data_start_index, data_end_index - 1);
      data_start_index = data_end_index;
    }
}

*/

