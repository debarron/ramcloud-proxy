#include "RCProxy.h"
#include <string>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

#define CLUSTER_NAME "_rctest_"

int main(int argc, char **argv){
  char *serviceLocator;
  double eTime;
  RCTable *insertTest;
  vector<RCEntry> entries;
  vector<RCEntry> multiEntries;

  serviceLocator = argv[1];

  eTime = 0.0;
  RCProxy rc(serviceLocator, CLUSTER_NAME);
 
  rc.createTable("Table1", 33);
  eTime += rc.elapsedSecsLog;
  rc.createTable("Table2", 33);
  eTime += rc.elapsedSecsLog;
  rc.createTable("Table3", 33);
  eTime += rc.elapsedSecsLog;
  cout << "Avg time to create table: " << (eTime/3.0)  << " secs.\n";
 
  eTime = 0.0;
  insertTest = rc.createTable("insertTest", 33);
  eTime = rc.elapsedSecsLog;
  cout << "Create table 'inserTest' with id " << insertTest->tableId << " took " << eTime << " secs.\n";

  eTime = 0.0;
  string key = "myKey";
  string value = "hello world";
  char *vp = new char[value.size()+1];
  strcpy(vp, value.c_str());

  RCEntry e(key, vp, value.length());
  entries.push_back(e);
  RCRelation simpleInput(insertTest, &entries);
  rc.push(&simpleInput);
  eTime = rc.elapsedSecsLog;
  cout << "Insert in 'inserTest' took " << eTime <<" secs\n";


  int valuesSize = 4;
  int values[4] = {1, 2, 3, 4};
  multiEntries.push_back(RCEntry(string("key1"), (char *)&values[0], sizeof(int)));
  multiEntries.push_back(RCEntry(string("key2"), (char *)&values[1], sizeof(int)));
  multiEntries.push_back(RCEntry(string("key3"), (char *)&values[2], sizeof(int)));
  multiEntries.push_back(RCEntry(string("key4"), (char *)&values[3], sizeof(int)));
  RCRelation multiInput(insertTest, &multiEntries);
  rc.push(&multiInput);
  eTime = rc.elapsedSecsLog;
  cout << "Insert in 'insertTest' " << valuesSize << " keys took " << eTime << " secs\n";

  vector<string> singleKey;
  singleKey.push_back(string("myKey"));
  RCRelation *singleKeyData = rc.pull(insertTest, singleKey);
  eTime = rc.elapsedSecsLog;
  int totalKeys = singleKeyData->entries->size();
  cout << "Pulling from 'insertTest' retrieved " << totalKeys << " keys took " << eTime << " secs\n";

  vector<string> singleKey2;
  singleKey2.push_back(string("key1"));
  singleKeyData = rc.pull(insertTest, singleKey2);
  eTime = rc.elapsedSecsLog;
  totalKeys = singleKeyData->entries->size();
  cout << "Pulling from 'insertTest' retrieved " << totalKeys << " keys took " << eTime << " secs\n";

  vector<string> singleKey3;
  singleKey3.push_back(string("key2"));
  singleKeyData = rc.pull(insertTest, singleKey3);
  eTime = rc.elapsedSecsLog;
  totalKeys = singleKeyData->entries->size();
  cout << "Pulling from 'insertTest' retrieved " << totalKeys << " keys took " << eTime << " secs\n";


  vector<string> multiKey;
  multiKey.push_back(string("key1"));
  multiKey.push_back(string("key2"));
  multiKey.push_back(string("key3"));
  multiKey.push_back(string("key4"));
  RCRelation *multiKeyData = rc.pull(insertTest, multiKey);
  eTime = rc.elapsedSecsLog;
  int totalMultiKey = multiKeyData->entries->size();
  cout << "Pulling (multiKey) from 'insertTest' retrieved " << totalMultiKey << " keys took " << eTime << " secs\n";

  vector<string> allKeys = rc.enumerateKeysFromTable(insertTest);
  cout << "Listing all keys from 'insertTest'\n";
  for(uint32_t i = 0; i < allKeys.size(); i++)
    cout << "has key: " << allKeys[i] << "\n";
  

  rc.dropTable("insertTest");
  rc.dropTable("Table1");
  rc.dropTable("Table2");
  rc.dropTable("Table3");

  return 0;
}
