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
    void _multi_read_request(uint64_t, vector<Entry>&, void*, Tub<ObjectBuffer>*);
    Relation *_multi_read_read_buffer(MultiReadObject*, Tub<ObjectBuffer>*, int);

  public:
    RCWrapper(string, string);
    int close();
    int count_entries(Relation&);
    double elapsed_time();
    uint64_t create_table(string, int);
    uint64_t get_table_id(string);
    void write(uint64_t, string, const char*, uint32_t);
    Entry read(uint64_t, string);
    Relation *read(Relation&);
    //int write(Relation&, int);
};
#endif
