/**
 * This a re-do of the project using
 * C++ datastructures
*/ 

#ifndef RCWRAPPER_H
#define RCWRAPPER_H

#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <map>
#include <cstdint>
#include <ctime>

#include "RamCloud.h"
#include "Tub.h"
#include "ObjectBuffer.h"

using namespace std;
using namespace RAMCloud;

typedef tuple<string, char*, uint32_t> Entry;
typedef tuple<const uint64_t*, const Entry*> MultiOpEntry;
typedef map<uint64_t, vector<Entry>> Relation;
typedef map<uint64_t, vector<Entry>>::iterator RelationIterator;

class RCWrapper{
  private:
    RamCloud *_client = NULL;
    CommandLineOptions _options;
    clock_t _start;
    clock_t _end;
    void _time_start();
    void _time_end();
    int _count_entries(Relation&);
    int _multiwrite_arr(MultiOpEntry*,int);
    void _multiwrite_request(void*, const uint64_t*, const Entry*);
    int _multiopt_count_success(void*,int,bool);
    void _multiread_append_relation(Relation*, Relation*);
    Relation *_multiread_arr(MultiOpEntry*, int, int*);
    void _multiread_request(void*, Tub<ObjectBuffer>*, const uint64_t*, const Entry*);
    Relation *_multiread_read_buffer(MultiReadObject*, Tub<ObjectBuffer>*, int);
    MultiOpEntry* _relation_to_multiop_entry(Relation &data);

  public:
    RCWrapper(string, string);
    int close();
    int count_entries(Relation&);
    double elapsed_time();
    uint64_t create_table(string, int);
    uint64_t get_table_id(string);
    void write(uint64_t, string, const char*, uint32_t);
    int write(Relation&, int = 1);
    Entry read(uint64_t, string);
    Relation *read(Relation&, int = 1, int* = NULL);
};


#endif
