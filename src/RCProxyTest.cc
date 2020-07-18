#include "RCProxy.h"
#include <iostream>
#include <vector>

#ifdef __cplusplus
#include <string>
#include <cstring>
#else
#include <string.h>
#endif

using namespace std;

#define CLUSTER_NAME "_rctest_"

void testSinglePushSinglePull(RCProxy &client){
  vector<RCEntry> number;
  vector<string> numKey;
  RCTable *dataTest;
  RCRelation *dataOut;
  uint32_t valueIn, valueOut;
  double timePush, timePull;
 
  valueIn = 69;
  dataTest = client.createTable("numTesting", 10);
  number.push_back(RCEntry("test", (char *)&valueIn, sizeof(uint32_t)));

  RCRelation dataInput (dataTest, &number);
  client.push(&dataInput);
  timePush = client.elapsedSecsLog;

  numKey.push_back("test");
  dataOut = client.pull(dataTest, numKey);
  timePull = client.elapsedSecsLog;

  valueOut = *((uint32_t *)(*dataOut->entries)[0].bytes);

  cout << ">> ramcloud-proxy SINGLE PUSH - SINGLE PULL EXPERIMENT\n"
    << "TABLE, VALUE_IN, VALUE_OUT, EQUAL, TIME_PUSH, TIME_PULL\n"
    << "numTesting, " 
    << valueIn << ", "
    << valueOut << ", "
    << (valueIn == valueOut) << ", "
    << timePush << ", "
    << timePull <<"\n";
}


void testingMultiPushSinglePull(RCProxy &client){
  vector<RCEntry> numbers;
  vector<string> numKeys;
  vector<string> pullKey;
  RCTable *dataTest;
  RCRelation *dataOut;
  double timePush, timePull;
  uint32_t valuesIn[5] = {1, 2, 3, 4, 5};
  uint32_t valueOut;
 
  dataTest = client.createTable("numbersTesting", 10);
  for(uint32_t i = 0; i < 5; i++){
    string key = "key" + to_string(i);
    numbers.push_back(RCEntry(key, (char *)&valuesIn[i], sizeof(uint32_t)));
    numKeys.push_back(key);
  }
  RCRelation dataInput (dataTest, &numbers);
  client.push(&dataInput);
  timePush = client.elapsedSecsLog;

  cout << ">> ramcloud-proxy MULTI PUSH - SINGLE PULL EXPERIMENT\n";
  cout << "TABLE, #VALUES_IN, TIME_PUSH\n";
  cout << "numbersTesting, 5, " << timePush <<"\n\n";

  cout << "TABLE, VALUE_OUT, VALUE_EXPECTED, IS_EQUAL, TIME_PULL\n";
  for(uint32_t i = 0; i < 5; i++){
    pullKey.clear();
    pullKey.push_back(numKeys[i]);

    dataOut = client.pull(dataTest, pullKey);
    valueOut = *((uint32_t *)(*dataOut->entries)[0].bytes);
    timePull = client.elapsedSecsLog;

    cout << "numbersTesting, " 
      << valueOut << ", "
      << valuesIn[i] << ", "
      << (valueOut == valuesIn[i]) << ", "
      << timePull << "\n";
  }
}

void testingMultiPushMultiPull(RCProxy &client){
  vector<RCEntry> numbers;
  vector<string> numKeys;
  RCTable *dataTest;
  RCRelation *dataOut;
  double timePush, timePull;
  uint32_t valuesIn[5] = {1, 2, 3, 4, 5};
  uint32_t valuesOut[5];
 
  dataTest = client.createTable("numbersTestingM", 10);
  for(uint32_t i = 0; i < 5; i++){
    string key = "key" + to_string(i);
    const char *theKey = key.data();
    string theKey_ = string(theKey);
    numbers.push_back(RCEntry(theKey_, (char *)&valuesIn[i], sizeof(uint32_t)));
    numKeys.push_back(theKey_);
  }
  RCRelation dataInput (dataTest, &numbers);
  client.push(&dataInput);
  timePush = client.elapsedSecsLog;

  cout << ">> ramcloud-proxy MULTI PUSH - MULTI PULL EXPERIMENT\n"
    << "TABLE, #VALUES_IN, TIME_PUSH\n"
    << "numbersTestingM, 5, " << timePush <<"\n\n";


  dataOut = client.pull(dataTest, numKeys);
  vector<RCEntry> &entries = *dataOut->entries;
  timePull = client.elapsedSecsLog;
  for(uint32_t i = 0; i < 5; i++){
    valuesOut[i] = *((uint32_t *)entries[i].bytes);
    cout << "READ K " << entries[i].key << " V " << entries[i].bytes << " L " << entries[i].bytesLength << "\n";
  }

  cout << "TABLE, #VALUES_FETCHED, TIME_PULL\n"
    << "numbersTestingM, " 
    << dataOut->entries->size() << ", "
    << timePull << "\n\n";
  
  cout << "VALUE_EXPECTED, VALUE_FETCHED, EQUAL\n";
  for(uint32_t i = 0; i < 5; i++)
    cout << valuesIn[i] << ", " << valuesOut[i] << ", " << (valuesIn[i] == valuesOut[i]) << "\n";
  

}


void testingListKeys(RCProxy &client){
  vector<string> *keys;
  RCTable *dataTest;
 
  dataTest = client.getTable("numbersTestingM");
  keys = client.listKeys(dataTest);

  for(uint32_t i = 0; i < keys->size(); i++)
    cout << "Keys [" << i <<"] = '" << (*keys)[i] << "'\n";
}



int main(int argc, char **argv){
  char *serviceLocator;
  double eTime;

  serviceLocator = argv[1];

  eTime = 0.0;
  RCProxy rc(serviceLocator, CLUSTER_NAME);
 
  rc.createTable("Table1", 33);
  eTime += rc.elapsedSecsLog;
  rc.createTable("Table2", 33);
  eTime += rc.elapsedSecsLog;
  rc.createTable("Table3", 33);
  eTime += rc.elapsedSecsLog;
  cout << "Avg time to create table: " << (eTime/3.0)  << " secs.\n\n";

  testSinglePushSinglePull(rc);
  testingMultiPushSinglePull(rc);
  testingMultiPushMultiPull(rc);
  testingListKeys(rc);

  return 0;
}
