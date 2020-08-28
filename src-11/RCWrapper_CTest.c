/**
 * This is a basic test on how to
 * use RCWrapper_C from a C file.
*/

#include "RCWrapper_C.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int TEST_create_and_get_table(const char *table_name){
  int result = 0;
  uint64_t table_id_1 = RCWrapper_C_create_table(table_name, 2);
  uint64_t table_id_1_t = RCWrapper_C_get_table_id("table_1");

  result = (table_id_1 == table_id_1_t) ? 1 : 0;

  if(result)
    fprintf(stdout, "## CREATE AND GET TABLE TEST PASSED\n");
  else
    fprintf(stdout, "## FAILED: CREATE AND GET TABLE TEST");

  return result;
}

int TEST_write_and_read(const char *table_name){
  int result = 0;
  char *value_test;
  uint32_t value_length = 0;
  uint64_t table_id;

  table_id = RCWrapper_C_create_table(table_name, 2);

  RCWrapper_C_write(table_id, "simple_key", "a_simple_value", 14);
  value_test = RCWrapper_C_read(table_id, "simple_key", &value_length);
  result = (value_length == 14 && (strcmp("a_simple_value", value_test) == 0)) ? 1 : 0;

  if(result)
    fprintf(stdout, "## WRITE AND READ TEST PASSED\n");
  else
    fprintf(stdout, "## FAILED: WRITE AND READ TEST, value_length:%d value:'%s'\n", value_length, value_test);
  
  return result;
}



int main(int argc, char **argv){
  RCWrapper_C_init("tcp:host=10.10.1.1,port=1110", "_test_");

  if(!TEST_create_and_get_table("test_table_1"))
    exit(1);
  else if(!TEST_write_and_read("test_table_2"))
    exit(1);
  
  // multi-write
  // multi-write-multi-table
  // read
  // mutti-read
  // multi-read-multi-table

  return 0;
}
