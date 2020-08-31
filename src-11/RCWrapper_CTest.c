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
  uint64_t table_id_1_t = RCWrapper_C_get_table_id(table_name);

  result = (table_id_1 == table_id_1_t) ? 1 : 0;

  if(result)
    fprintf(stdout, "## CREATE AND GET TABLE TEST PASSED\n");
  else
    fprintf(stdout, "## FAILED: CREATE AND GET TABLE TEST table_id:%d table_id_t:%d\n", table_id_1, table_id_1_t);

  return result;
}

int TEST_write_and_read(const char *table_name){
  int result = 0;
  char *value_test;
  uint32_t value_length = 0;
  uint64_t table_id;

  table_id = RCWrapper_C_create_table(table_name, 2);

  RCWrapper_C_single_write(table_id, "simple_key", "a_simple_value", 14);
  value_test = RCWrapper_C_single_read(table_id, "simple_key", &value_length);
  result = (value_length == 14 && (strcmp("a_simple_value", value_test) == 0)) ? 1 : 0;

  if(result)
    fprintf(stdout, "## WRITE AND READ TEST PASSED\n");
  else
    fprintf(stdout, "## FAILED: WRITE AND READ TEST, value_length:%d value:'%s'\n", value_length, value_test);
  
  return result;
}

int TEST_multiwrite_multiread(const char *table_name){
  int result_writes = 0;
  int result_reads = 0;

  uint64_t table_id = RCWrapper_C_create_table(table_name, 2);
  const char *a_key = "a_simple_key";
  const char *a_value = "a_sample_value";
  char buffer[100];

  //15=> key
  //17 => value

  RCRecord records[4];
  RCRecord *records_read;

  for (int i = 0; i < 4; i++){
    char *key = (char *) malloc(sizeof(char) * 15);
    char *value = (char *) malloc(sizeof(char) * 17);
    
    sprintf(key, "%s_%d\0", a_key, i+1);
    sprintf(value, "%s_%d\0", a_value, i+1);

    records[i] = {table_id, strlen(key), strlen(value), key, value};
  }
  result_writes = (RCWrapper_C_multi_write(records, 4, 1) == 4) ? 1: 0;
  for(int i = )

  uint32_t vl0, vl1, vl2, vl3;
  char *v0 = RCWrapper_C_single_read(table_id, records[0].key, &vl0);
  char *v1 = RCWrapper_C_single_read(table_id, records[1].key, &vl1);
  char *v2 = RCWrapper_C_single_read(table_id, records[2].key, &vl2);
  char *v3 = RCWrapper_C_single_read(table_id, records[3].key, &vl3);
  fprintf(stdout, "v0:'%s' v1:'%s' v2:'%s' v3:'%s'\n", v0, v1, v2, v3);


  result_reads = 0;
  records_read = RCWrapper_C_multi_read(records, 4, 1);
  for(int i = 0; i < 4; i++)
    result_reads += (strcmp(records[i].value, records_read[i].value) == 0) ? 1 : 0;
  
  if(result_writes) fprintf(stdout, "## MULTI-WRITE TEST PASSED\n");
  else fprintf(stdout, "## FAILED: MULTI-WRITE TEST\n");

  if(result_reads) fprintf(stdout, "## MULTI-READ TEST PASSED\n");
  else fprintf(stdout, "## FAILED: MULTI-READ TEST\n");
  
  return ((result_writes + result_reads) == 2) ? 1 : 0;
}


int main(int argc, char **argv){
  RCWrapper_C_init("tcp:host=10.10.1.1,port=1110", "_test_");

  if(!TEST_create_and_get_table("test_table_1"))
    exit(1);
  else if(!TEST_write_and_read("test_table_2"))
    exit(1);
  else if(!TEST_multiwrite_multiread("test_table_3"))
    exit(1);
  
  // multi-write
  // multi-write-multi-table
  // read
  // mutti-read
  // multi-read-multi-table

  return 0;
}
