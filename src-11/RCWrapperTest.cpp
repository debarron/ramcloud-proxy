#include <iostream>
#include <cstdint>
#include <algorithm>
#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;

void TEST_create_table(RCWrapper &wrapper){
  uint64_t table_id = wrapper.create_table("test_table", 2);
  uint64_t table_id_test = wrapper.get_table_id("test_table");
  string test_result = (table_id == table_id_test) ? "PASSED" : "FAILED";

  cout << "## CREATE TABLE TEST "
    << "table_id = " << table_id << " table_id_test = " << table_id_test << endl
    << "TEST " << test_result << endl << endl;
}

void TEST_single_write(RCWrapper &wrapper, string table_name){
  uint64_t table_id = wrapper.create_table(table_name, 2);

  string key = "test_key";
  string value = "This is a message from C11";

  wrapper.write(table_id, key, value.data(), value.length());
  cout << "## SINGLE WRITE TEST ";
  cout << " PASSED\n";
}

void TEST_single_read(RCWrapper &wrapper, string table_name){
  char *value_read;
  int value_length;
  uint64_t table_id;
  string key = "test_key";
  string value = "This is a message from C11";

  table_id = wrapper.create_table(table_name, 2);
  wrapper.write(table_id, key, value.data(), value.length());
  tie(ignore, value_read, value_length) = wrapper.read(table_id, key);

  bool same_value = memcmp(value.data(), value_read, value_length) == 0;
  bool same_value_length = value.length() == value_length;
  cout << "## SINGLE READ TEST " <<
    "SAME VALUE: [" << value << "," << value_read << "] " << same_value << endl
    << " SAME LENGTH: [" << value.length() << "," << value_length << "] " << same_value_length << endl;
}

void TEST_multi_write(RCWrapper &wrapper, string table_name){
  uint64_t table_id;
  string key, value;
  Relation input_data;
  vector<Entry> keys;
  int result;
  
  table_id = wrapper.create_table(table_name, 2);
  key = "the_key_";
  value = "value_example_";
  for (int i = 1; i < 5; i++){
    string new_key = key + to_string(i);
    string new_value = value + to_string(i);
    char *new_value_cstr = (char *)malloc(sizeof(char) * new_value.length());
    memcpy(new_value_cstr, new_value.data(), new_value.length());

    keys.push_back(make_tuple(new_key, new_value_cstr, new_value.length()));
  }

  input_data[table_id] = keys;
  result = wrapper.write(input_data);

  bool same_key_count_test = result == keys.size();
  cout << "## TEST MULTI-WRITE SAME KEY COUNT " << same_key_count_test << endl;
}

void TEST_multi_read(RCWrapper &wrapper, string table_name){
  uint64_t table_id;
  int successful_reads;
  Relation *output_data;
  Relation input_data;
  vector<Entry> keys;

  table_id = wrapper.create_table(table_name, 2);
  for (int i = 1; i < 5; i++){
    string new_key = "the_key_" + to_string(i);
    string new_value = "a_value_sample_" + to_string(i);
    char *new_value_cstr = (char *)malloc(sizeof(char) * new_value.length());
    memcpy(new_value_cstr, new_value.data(), new_value.length());

    wrapper.write(table_id, new_key, new_value_cstr, new_value.length());
    keys.push_back(make_tuple(new_key, new_value_cstr, new_value.length()));
  }
  input_data[table_id] = keys;

  successful_reads = 0;
  output_data = wrapper.read(input_data, 1, &successful_reads);
  bool same_key_count_test = wrapper.count_entries(input_data) == wrapper.count_entries(*output_data);
  cout << "## TEST MULTI-WRITE SUCCESSFUL READS " << successful_reads 
    << " SAME KEY COUNT " << same_key_count_test << endl;
}


int main(int argc, char **argv){
  RCWrapper wrapper("tcp:host=10.10.1.1,port=1110", "main");

  int offset = 0;

  TEST_create_table(wrapper, "test_table_" + to_string(offset++));
  TEST_single_write(wrapper, "test_table_" + to_string(offset++));
  TEST_single_read(wrapper, "test_table_" + to_string(offset++));
  TEST_multi_write(wrapper, "test_table_" + to_string(offset++));
  TEST_multi_read(wrapper, "test_table_" + to_string(offset++));

  return 0;
}
