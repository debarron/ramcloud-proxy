#include <iostream>
#include <cstdint>
#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;


void test_table(RCWrapper &wrapper){
  uint64_t table_id = wrapper.create_table("test_table", 2);
  uint64_t table_id_test = wrapper.get_table_id("test_table");
  string test_result = (table_id == table_id_test) ? "PASSED" : "FAILED";

  cout << "## TABLE TEST\n"
    << "table_id = " << table_id << " table_id_test = " << table_id_test << endl
    << "TEST " << test_result << endl << endl;
}

void test_single_write_single_read(RCWrapper &wrapper){
  uint64_t table_id = wrapper.create_table("test_single_write_4", 2);

  cout << "## SINGLE WRITE TEST ";
  string key = "test_key";
  string value = "This is a message from C11";

  wrapper.write(table_id, key, value.data(), value.length());
  cout << " PASSED\n";


  cout << "## SINGLE READ TEST ";
  char *value_read;
  int value_length;
  string value_read_str;

  tie(ignore, value_read, value_length) = wrapper.read(table_id, key);
  value_read_str = string(value_read);
  bool same_value = memcmp(value.data(), value_read, value_length) == 0;
  bool same_value_length = value.length() == value_length;
  cout << " SAME VALUE: [" << value << "," << value_read << "] " << same_value << endl
    << " SAME LENGTH: [" << value.length() << "," << value_length << "] " << same_value_length << endl;

}

void test_single_write_multi_read(RCWrapper &wrapper){
  uint64_t table_id = wrapper.create_table("test_single_write_multi_read_1", 2);

  cout << "## WRITE TEST ";
  string key = "test_key";
  string value = "This is a message from C11 it ";
  vector<Entry> keys;

  for (int i = 1; i < 5; i++){
    string new_key = key + to_string(i);
    string new_value = value + to_string(i);

    Entry new_entry = make_tuple(new_key, new_value.data(), new_value.length());
    keys.push_back(new_entry);
    wrapper.write(table_id, new_key, new_value.data(), new_value.length());
  }
  cout << " PASSED\n";


  cout << "## MULTI READ TEST ";
  char *value_read;
  int value_length;
  string value_read_str;

  tie(ignore, value_read, value_length) = wrapper.read(table_id, key);
  value_read_str = string(value_read);
  bool same_value = memcmp(value.data(), value_read, value_length) == 0;
  bool same_value_length = value.length() == value_length;
  cout << " SAME VALUE: [" << value << "," << value_read << "] " << same_value << endl
    << " SAME LENGTH: [" << value.length() << "," << value_length << "] " << same_value_length << endl;

}



int main(int argc, char **argv){
  RCWrapper wrapper("tcp:host=10.10.1.1,port=1110", "main");

  test_table(wrapper);
  test_single_write_single_read(wrapper);
  test_single_write_multi_read(wrapper);

  return 0;
}
