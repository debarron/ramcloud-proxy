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

MultiOpEntry* RCWrapper::_slice_relation_from(Relation &data, int start_index, int end_index){
  int multi_write_count = (end_index - start_index) + 1;
  MultiOpEntry *entries = new MultiOpEntry[multi_write_count]();
  int entries_index = -1;
  int current_index = 0;
  
  for (RelationIterator it = data.begin(); it != data.end() && current_index <= end_index; it++){
    vector<Entry> *entries_pointer = &(*it).second;

    if(start_index > (current_index + entries_pointer->size() - 1)){
      current_index += entries_pointer->size() -1;
      continue;
    }

    int offset = start_index - current_index;
    vector<Entry>::iterator entry_it;
    for(entry_it = entries_pointer->begin() + offset; 
        entry_it != entries_pointer->end() && current_index <= end_index; 
        entry_it++, current_index++){

      entries[++entries_index] = make_pair(&(*it).first, &(*entry_it));
    } 
  }

  return entries;
}


/***********/
// WRITE
/***********/
void RCWrapper::write(uint64_t table_id, string entry_key, const char *entry_value, uint32_t entry_value_length){
  uint64_t version;

  this->_client->write(
      table_id, entry_key.data(), entry_key.length(),
      entry_value, entry_value_length,
      NULL, &version, false);
}

int RCWrapper::write(Relation &data, int steps){
  int total_entries = _count_entries(data);
  int step_size = total_entries / steps;
  int data_start_index = 0;
  int data_end_index = 0;
  int data_write_count = 0;

  for(int i = 1; i <= steps; i++){
    data_end_index = (i == steps) ? total_entries : i * step_size;

    MultiOpEntry *arr = _slice_relation_from(data, data_start_index, data_end_index -1);
    int arr_length = data_end_index - data_start_index;

    data_write_count += _multiwrite_arr(arr, arr_length);
    data_start_index = data_end_index;
    delete[] arr;
  }

  return data_write_count;
}

int RCWrapper::_multiwrite_arr(MultiOpEntry *entries, int entries_length){
  MultiWriteObject request[entries_length];
  MultiWriteObject *request_pointer[entries_length];

  for(int i = 0; i < entries_length; i++){
    const uint64_t *table_id_p = get<0>(entries[i]);
    const Entry *entry_p = get<1>(entries[i]);

    _multiwrite_request((void *)&request[i], table_id_p, entry_p);
    request_pointer[i] = &request[i];
  }

  this->_client->multiWrite(request_pointer, entries_length);
  return _multiwrite_count_success(request, entries_length);
}

void RCWrapper::_multiwrite_request(void *memory_address, const uint64_t *table_id, const Entry *e){
  string key;
  char *value;
  uint32_t value_length;

  tie(key, value, value_length) = *e;
  new(memory_address) MultiWriteObject(*table_id, key.data(), key.length(), value, value_length, NULL);
}

int RCWrapper::_multiwrite_count_success(MultiWriteObject *request, int request_count){
  int succes_count = 0;
  for(int i = 0; i < request_count; i++)
    succes_count += (request[i].status == STATUS_OK) ? 1 : 0;

  return succes_count;
}

/**********/
// READ
/**********/
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

Relation *RCWrapper::read(Relation &data, int steps){
  int total_entries = _count_entries(data);
  int step_size = total_entries / steps;
  int data_start_index = 0;
  int data_end_index = 0;

  Relation *result = new Relation();
  for(int i = 1; i <= steps; i++){
    data_end_index = (i == steps) ? total_entries : i * step_size;

    MultiOpEntry *arr = _slice_relation_from(data, data_start_index, data_end_index -1);
    int arr_length = data_end_index - data_start_index;

    Relation *multiread_result = _multiread_arr(arr, arr_length);
    _multiread_append_relation(result, multiread_result);
    data_start_index = data_end_index;

    delete[] arr;
  }

  return result;
}

void RCWrapper::_multiread_append_relation(Relation *dest, Relation *source){
  for(RelationIterator it = source->begin(); it != source->end(); ++it){
    const uint64_t *it_key = &it->first;
    const vector<Entry> *entries = &it->second;

    if(dest->find(*it_key) == dest->end())
      dest->insert(make_pair(*it_key, *entries));
    else{
      vector<Entry> *current_entries = &dest->at(*it_key);
      current_entries->insert(current_entries->end(), entries->begin(), entries->end());
    }
  }
}

Relation *RCWrapper::_multiread_arr(MultiOpEntry *arr, int arr_length){
  MultiReadObject request[arr_length];
  MultiReadObject *request_pointer[arr_length];
  Tub<ObjectBuffer> request_buffer[arr_length];
  int request_index = 0;
  
  for(int i = 0; i < arr_length; i++){
    const uint64_t *table_id_p = get<0>(arr[i]);
    const Entry *entry_p = get<1>(arr[i]);

    _multiread_request((void *)&request[request_index], &request_buffer[request_index], table_id_p, entry_p);
    request_pointer[request_index] = &request[request_index];
    request_index++;
  }

  this->_client->multiRead(request_pointer, arr_length);
  Relation *result = _multiread_read_buffer(request, request_buffer, arr_length);

  return result;
}


void RCWrapper::_multiread_request(void *memory_address, Tub<ObjectBuffer> *buffer, const uint64_t *table_id, const Entry *e){ 
  string key;
  tie(key, ignore, ignore) = *e;
  new(memory_address) MultiReadObject(*table_id, key.data(), key.length(), buffer);
}


Relation *RCWrapper::_multiread_read_buffer(MultiReadObject *request, Tub<ObjectBuffer> *buffer, int buffer_count){
  ObjectBuffer *buffer_reader;
  uint64_t table_id = request[0].tableId;
  vector<Entry>* entries = new vector<Entry>();
  Relation *result = new Relation();
  uint32_t data_length;

  result = new Relation();
  for(int i = 0; i < buffer_count; i++){
    if(table_id != request[i].tableId){
      result->insert(make_pair(table_id, *entries));
      entries = new vector<Entry>();
      table_id = request[i].tableId;
    }

    buffer_reader = buffer[i].get();
    const char *buffer_key = reinterpret_cast<const char *>(buffer_reader->getKey(0));
    const char *buffer_data = reinterpret_cast<const char *>(buffer_reader->getValue(&data_length));
    int buffer_key_length = buffer_reader->getKeyLength(0);

    char *data_key = (char *) malloc(sizeof(char) * buffer_key_length);
    char *data = (char *)malloc(sizeof(char) * data_length);
    memcpy(data_key, buffer_key, buffer_key_length);
    memcpy(data, buffer_data, data_length);

    string data_key_str (buffer_key);
    entries->push_back(make_tuple(data_key_str, data, data_length));
  }

  result->insert(make_pair(table_id, *entries));
  return result;
}

/*
Relation *RCWrapper::read(MultiOpEntry *entries, int entries_length){
  MultiReadObject request[entries_length];
  MultiReadObject *request_pointer[entries_length];
  Tub<ObjectBuffer> request_buffer[entries_length];
  int request_index = 0;
  
  for(int i = 0; i < entries_length; i++){
    uint64_t *table_id_p;
    Entry *entry_p;
    tie(table_id_p, entry_p) = entries[i];

    _multi_read_request(*table_id_p, *entry_p, (void *)&request[request_index], &request_buffer[request_index]);
    request_pointer[request_index] = &request[request_index];
    request_index++;
  }

  this->_client->multiRead(request_pointer, total_entries);
  Relation *result = _multi_read_read_buffer(request, request_buffer, entries_length);

  return result;
}


Relation *RCWrapper::read(Relation &data, int steps = 1){
  int total_entries = _count_entries(data);

  MultiReadObject request[total_entries];
  MultiReadObject *request_pointer[total_entries];
  Tub<ObjectBuffer> request_buffer[total_entries];
  int request_index = 0;
  
  for(RelationIterator it = data.begin(); it != data.end(); ++it){

    vector<Entry> entry_i = (*it).second;
    for(vector<Entry>::iterator it_entry = entry_i.begin(); it_entry != entry_i.end(); ++it_entry){
      _multi_read_request((*it).first, entry_i, (void *)&request[request_index], &request_buffer[request_index]);
      request_pointer[request_index] = &request[request_index];
      request_index++;
    }
  }

  this->_client->multiRead(request_pointer, total_entries);
  Relation *result = _multi_read_read_buffer(request, request_buffer, total_entries);

  return result;
}



*/
