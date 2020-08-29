#include "RCWrapper_C.h"
#include "RCWrapper.h"

using namespace std;
RCWrapper *wrapper = NULL;

void RCW_free_relation(Relation *r){
  for(RelationIterator it = r->begin(); it != r->end(); ++it)
    (it->second).clear();
  r->clear();
  delete r;
}

Relation *RCW_record_to_relation(RCRecord *records, int records_length){
  Relation *relation = new Relation();
  vector<Entry> *entries = new vector<Entry>();
  uint64_t previous_table_id = records[0].table_id;

  for (int i = 0; i < records_length; i++){
    if (previous_table_id != records[i].table_id){
      relation->insert(make_pair(previous_table_id, *entries));
      previous_table_id = records[i].table_id;
      entries = new vector<Entry>();
    }

    string _key = string(records[i].key);
    char *_value = records[i].value;
    uint32_t _value_length = records[i].value_length;
    entries->push_back(make_tuple(_key, _value, _value_length));
  }

  relation->insert(make_pair(previous_table_id, *entries));
  return relation;
}

// TODO Test!
RCRecord *RCW_relation_to_record(Relation *r){
  RCRecord *result;
  int result_index = 0;
  int total_entries = wrapper->count_entries(*r);

  result = new RCRecord[total_entries]();
  for(RelationIterator it = r->begin(); it != r->end(); ++it){
    vector<Entry> *e = &(it->second);
    for(vector<Entry>::iterator vit = e->begin(); vit != e->end(); ++vit){
      string _key;
      char *_value;
      uint32_t _value_length;
      tie(_key, _value, _value_length) = (*vit);

      char *_cstr_key = new char[_key.length() + 1]();
      memcpy(_cstr_key, _key.data(), _key.length());
      _cstr_key[_key.length()] = '\0';
      result[result_index++] = {it->first, _key.length()+1, _value_length, _cstr_key, _value};
    }
  }

  return result;
}


void RCWrapper_C_init(const char *service_locator, const char *cluster_name){
  string _service_locator = string(service_locator);
  string _cluster_name = string(cluster_name);

  if(wrapper == NULL)
    wrapper = new RCWrapper(_service_locator, _cluster_name);
}

uint64_t RCWrapper_C_create_table(const char *table_name, int server_span){
  string _table_name = string(table_name);
  return wrapper->create_table(_table_name, server_span);
}

uint64_t RCWrapper_C_get_table_id(const char *table_name){
  string _table_name = string(table_name);
  return wrapper->get_table_id(_table_name);
}

void RCWrapper_C_single_write(uint64_t table_id, const char *key, const char *value, uint32_t value_length){
  string _key = string(key);
  wrapper->write(table_id, _key, value, value_length);
}

int RCWrapper_C_multi_write(RCRecord *records, int records_length, int steps){
  int success_writes = 0;

  Relation *r = RCW_record_to_relation(records, records_length);
  wrapper->write(*r, steps);
  success_writes = wrapper->count_entries(*r);

  RCW_free_relation(r);
  return success_writes;
}


char *RCWrapper_C_single_read(uint64_t table_id, const char *key, uint32_t *value_length){
  char *_value;
  string _key;

  _key = string(key);
  tie(ignore, _value, *value_length)= wrapper->read(table_id, _key);
  return _value;
}

// THIS ONE!!
RCRecord *RCWrapper_C_multi_read(RCRecord *records, int records_length, int steps){
  int success_reads = 0;

  Relation *r = RCW_record_to_relation(records, records_length);
  Relation *data_read = wrapper->read(*r, steps, &success_reads);
  return RCW_relation_to_record(data_read); 
}


