#include <iostream>
#include <cstdint>
#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;

int main(int argc, char **argv){
  RCWrapper wrapper("tcp:host=10.10.1.1,port=1110", "main");

  uint64_t table_id = wrapper.create_table("test1", 2);
  uint64_t table_id_test = wrapper.get_table_id("test1");
  cout << "## TABLE TEST\n"
    << "table_id = " << table_id << " table_id_test = " << table_id_test << endl
    << "TEST " << (table_id == table_id_test) ? "PASSED" : "FAILED" << endl << endl;
}
