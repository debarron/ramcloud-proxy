#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include "RamCloud.h"
#include "Buffer.h"
#include "Object.h"
#include "ObjectBuffer.h"
#include "ClientException.h"
#include "MultiRead.h"
#include "Tub.h"
#include "TableEnumerator.h"

using namespace std;
using namespace RAMCloud;

int rc_select_cli_options_validate(int argc){
  int option;

  if(argc == 4) option = 1;
  else if(argc == 5) option = 2;
  else option = -1;

  return option;
}

int main(int argc, char **argv){
  // Parse instruction
  // Case 1: service_locator cluster_name * ==> list_all_tables
  // Case 1: service_locator cluster_name table_name ==> list_all_keys_from_table
  // case 2: service_locator cluster_name * * ==> list_all_tables_with_all_keys
  int option;
  string table_name;
  bool with_keys;

  RamCloud *client;
  CommandLineOptions cli_options;
  uint64_t table_id;

  option = rc_select_cli_options_validate(argc);
  if(!option){
    cout << "rc-select \n>> ERROR wrong list of arguments\n";
    return 1;
  }

  table_name = argv[3];
  with_keys = (option == 2) ? 1 : 0;
  if(table_name.compare("*") == 0){
    cout << "rc-select \n>> ERROR table_name not valid '*'\n";
    return 1;
  }

  cli_options.coordinatorLocator = argv[1];
  cli_options.clusterName = argv[2];
  client  = new RamCloud(&cli_options);
  table_id = client->getTableId(table_name.data());

  cout << "ramcloud-select \n>> Connected to RAMCloud "
    << "(" << cli_options.coordinatorLocator 
    << ", " << cli_options.clusterName << ")\n"
    << ">> READING TABLE: " << table_name << "[" << table_id << "]\n";


  vector<string> *keys;
  uint32_t bufferLength, keyLength;
  const void *buffer;
  TableEnumerator enumerator (*client, table_id, true);
  keys = new vector<string>();
  while(enumerator.hasNext()){
    keyLength = bufferLength = 0;
    enumerator.next(&bufferLength, &buffer);
    Object enumObject (buffer, bufferLength);

    keyLength = enumObject.getKeyLength();
    string key = string(reinterpret_cast<const char *>(enumObject.getKey()), keyLength);
    keys->push_back(key);
  }

  for(vector<string>::iterator it = keys->begin(); it != keys->end(); ++it) 
    cout << *it << endl;
  
  return 0;
}
