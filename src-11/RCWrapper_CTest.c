/**
 * This is a basic test on how to
 * use C_RCPorxy from a C file.
*/

#include "C_RCProxy.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CLUSTER_NAME_TEST "test"

int main(int argc, char **argv){
  C_RCProxy_init("tcp:host=10.10.1.1,port=1110", "main");


  fprintf(stdout, ">> TABLE ID TEST\n");
  uint64_t table_id_1 = C_RCProxy_create_table("test", 3);
  uint64_t table_id_2 = C_RCProxy_get_table_id("test");

  if(table_id_1 == table_id_2)
    fprintf(stdout, "\t> + Passed the test %d == %d", table_id_1, table_id_2);
  else
    fprintf(stdout, "\t> - Failed the test %d != %d", table_id_1, table_id_2);

  fprintf(stdout, "\n>> END TABLE ID TEST\n\n");


  fprintf(stdout, ">> WRITE AND READ TEST\n");
  char name[7] = "daniel";
  int name_len = 7;
  char *name_p;
  int name_p_len;
  C_RCProxy_push(table_id_1, "test", "the_name", name, name_len);
  C_RCProxy_pull(table_id_1, "test", "the_name", &name_p, &name_p_len);

  fprintf(stdout, "\t> Bytes comparison %d ? %d\n", name_len, name_p_len);
  if(strcmp(name, name_p) == 0)
    fprintf(stdout, "\t> + Passed the test %s == %s", name, name_p);
  else
    fprintf(stdout, "\t> - Failed the test %s != %s", name, name_p);

  fprintf(stdout, "\n>> END WRITE AND READ TEST\n\n");

  return 0;
}
