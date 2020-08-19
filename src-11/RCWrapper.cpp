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
#include <ctime>

#include "RAMCloud.h"
#include "ClientException.h"

#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;
using namespace RCWrapper;

void RCWrapper::_time_start(){_start = clock();}
void RCWrapper::_time_end(){_end = clock();}
double RCWrapper::_elapsed_time(){ double(_end - _start)/CLOOCKS_PER_SEC;}
double RCWrapper::elapsed_time(){ return _elapsed_time();}
int RCWrapper::_count_entries(Relation &data){
  int total_entries = 0;

  for(RelationIterator it = data.begin(); it != data.end(); it++)
    total_entries += ((vector<Entry>)it->second).size();

  return total_entries;
}

int RCWrapper::RCWrapper(string service_locator, string cluster_name){
  if(client == NULL){
    this->options.coordinatorLocator = service_locator;
    this->options.clusterName = cluster_name;
    this->client = new RamCloud(&this->options);
  }
}

// TODO Implement closing the connection to RAMCloud
int RCWrapper::close(){
  return 1;
}

// ****************************
// TABLE OPERATIONS
// ****************************
uint64_t RCWrapper::create_table(string table_name, int server_span){
  return client->creeateTable(table_name, server_span);
}

uint64_t RCWrapper::get_table_id(string table_name){
  uint64_t table_id;

  try{
    table_id = client->creeateTable(table_name, server_span);
  }catch(TableDoesntExistsException &e){
    table_id = -1;
  }

  return table_id;
}
// ****************************
// TABLE OPERATIONS
// ****************************


// ****************************
// WRITE OPERATIONS
// ****************************
int RCWrapper::write(uint64_t table_id, string entry_key, char *entry_value, uint32_t entry_value_length){
  uint64_t version;

  this->client->write(
      table_id, entry_key.data(), entry_key.length(),
      entry_value, entry_value_length,
      NULL, &version, false);
}

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



