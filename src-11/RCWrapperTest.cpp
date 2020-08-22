#include <iostream>
#include <cstdint>
#include <algorithm>
#include "RCWrapper.h"

using namespace std;
using namespace RAMCloud;

void test_table(RCWrapper &wrapper, string table_name){
  uint64_t table_id = wrapper.create_table(table_name, 2);
  uint64_t table_id_test = wrapper.get_table_id("test_table");
  string test_result = (table_id == table_id_test) ? "PASSED" : "FAILED";

  cout << "## TABLE TEST\n"
    << "table_id = " << table_id << " table_id_test = " << table_id_test << endl
    << "TEST " << test_result << endl << endl;
}

void test_single_write_single_read(RCWrapper &wrapper, string table_name){
  uint64_t table_id = wrapper.create_table(table_name, 2);

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

string Entry_get_key(Entry e){
  return get<0>(e);
}
char *Entry_get_value(Entry e){
  return get<1>(e);
}

bool string_compare(string s1, string s2){
  return s1.compare(s2) == 0;
}

// 7 is based on the for loop, only adding
// one char to a 6-char string
bool char_star_compare(char *c1, char *c2){
  return memcmp(c1, c2, 7) == 0;
}

bool _compare_vectors_keys(vector<Entry> v1, vector<Entry> v2){
  vector<string> keys1;
  vector<string> keys2;

  keys1.resize(v1.size());
  transform(v1.begin(), v1.end(), keys1.begin(), Entry_get_key);
  sort(keys1.begin(), keys1.end());

  keys2.resize(v2.size());
  transform(v2.begin(), v2.end(), keys2.begin(), Entry_get_key);
  sort(keys2.begin(), keys2.end());

  return equal(keys1.begin(), keys1.end(), keys2.begin(), string_compare);
}

void do_single_read_and_print(RCWrapper &wrapper, uint64_t table_id, string key){
  char *value_read;
  char *value_print;
  int value_length;
  string value_read_str;

  tie(ignore, value_read, value_length) = wrapper.read(table_id, key);
  value_print = (char *) malloc(sizeof(char) * value_length + 1);
  memcpy(value_print, value_read, value_length);
  value_print[value_length] = '\0';

  cout << "## SINGLE READ " <<
    " table_id: " << table_id << " key: " << key <<
    " value: " << value_print << endl;
}

void test_single_write_multi_read(RCWrapper &wrapper, string table_name){
  uint64_t table_id = wrapper.create_table(table_name, 2);

  cout << "## WRITE TEST ";
  string key = "test_key";
  string value = "***** ";
  vector<Entry> keys;

  for (int i = 1; i < 5; i++){
    string new_key = key + to_string(i);
    string new_value = value + to_string(i);
    char *new_value_cstr = (char *)malloc(sizeof(char) * new_value.length());
    memcpy(new_value_cstr, new_value.data(), new_value.length());

    Entry new_entry = make_tuple(new_key, new_value_cstr, new_value.length());
    keys.push_back(new_entry);
    wrapper.write(table_id, new_key, new_value.data(), new_value.length());
  }
  cout << " PASSED\n";

  cout << "## TEST SINGLE READ\n";
  do_single_read_and_print(wrapper, table_id, "test_key1");
  do_single_read_and_print(wrapper, table_id, "test_key2");
  do_single_read_and_print(wrapper, table_id, "test_key3");
  do_single_read_and_print(wrapper, table_id, "test_key4");

  cout << "## MULTI READ TEST \n";
  Relation data_read;
  data_read[table_id] = keys;

  Relation *output = wrapper.read(data_read);
  int output_entries = wrapper.count_entries(*output);
  cout << "\t >> OUTPUT COUNT " << output_entries << endl;

  bool same_vector_keys = true;
  for(RelationIterator it = data_read.begin(); it != data_read.end(); ++it){
    vector<Entry> entries = (*it).second;
    vector<Entry> entries_from_ramcloud = (*output)[(*it).first];

    same_vector_keys = same_vector_keys && _compare_vectors_keys(entries, entries_from_ramcloud);
  }
  cout << "\t >> SAME KEYS IN BOTH VECTORS " << same_vector_keys << endl;
}



int main(int argc, char **argv){
  RCWrapper wrapper("tcp:host=10.10.1.1,port=1110", "main");

  int offset = 4;
  test_table(wrapper, "test_table_" + to_string(offset++));
  test_single_write_single_read(wrapper, "test_table_" + to_string(offset++));
  test_single_write_multi_read(wrapper, "test_table_" + to_string(offset++));

  return 0;
}
