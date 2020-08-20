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

using namespace std;
using namespace RAMCloud;

namespace RCWrapper{
  typedef tuple<string, char*, uint32_t> Entry;
  typedef map<uint64_t, vector<Entry>> Relation;
  typedef map<uint64_t, vector<Entry>>::iterator RelationIterator;

  class RCWrapper;
}

class RCWrapper::RCWrapper{
  private:
    RamCloud *_client = NULL;
    CommandLineOptions _options;
    clock_t _start;
    clock_t _end;
    void _time_start();
    void _time_end();
    int _count_entries(RCWrapper::Relation&);

  public:
    RCWrapper(string, string);
    int close();
    double elapsed_time();
    uint64_t create_table(string, int);
    uint64_t get_table_id(string);
    /*int write(Relation&, int);
    int write(uint64_t, string, char*, uint32_t);
    Relation *read(Relation&);
    Relation *read(uint64_t, string);*/
};
#endif
