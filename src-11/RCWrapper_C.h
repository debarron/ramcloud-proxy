#ifndef RC_WRAPPER_C_H
#define RC_WRAPPER_C_H

#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
#include "RCWrapper.h"

#include <string>
#include <ctime>
#include <vector>
#include <cstring>

extern "C" {
#endif

typedef struct{
  uint64_t table_id;
  uint32_t key_length;
  uint32_t value_length;
  char *key;
  char *value;
} RCRecord;

void RCWrapper_C_init(const char *service_locator, const char *cluster_name);
uint64_t RCWrapper_C_create_table(const char *table_name, int server_span);
uint64_t RCWrapper_C_get_table_id(const char *table_name);

void RCWrapper_C_single_write(uint64_t table_id, const char *key, const char *value, uint32_t value_length);
int RCWrapper_C_multi_write(RCRecord *records, int records_length, int steps);

char *RCWrapper_C_single_read(uint64_t table_id, const char *key, uint32_t *value_length);
RCRecord *RCWrapper_C_multi_read(RCRecord *records, int records_length, int steps);



#ifdef __cplusplus
}
#endif

#endif


