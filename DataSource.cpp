#include <WString.h>

#include "DataSource.hpp"

// DataSource.cpp

DataSource::DataSource(){}

DataSource::DataSource(
  const char* name, 
  const char* description,
  const char* endpoint,
  const char* dataType,
  const char* dataPollingType){

  this->name = name;
  this->description = description;
  this->endpoint = endpoint;
  this->dataType = dataType;
  this->dataPollingType = dataPollingType;
  this->timeBetweenPush = DEFAULT_TIME_BETWEEN_PUSH;
  this->lastPush = 0;
}

void DataSource::stopPush(){
  this->timeBetweenPush = -1;
  this->lastPush = 0;
}

void DataSource::startPush(){
  this->timeBetweenPush = DEFAULT_TIME_BETWEEN_PUSH;
  this->lastPush = 0;
}

void DataSource::setPush(unsigned long timeBetweenPush){
  this->timeBetweenPush = timeBetweenPush;
  this->lastPush = 0;
}


String DataSource::toString(){
  return "name : " + this->name + ", serializedState : " + this->serializedState + ", description : " + this->description + ", endpoint : " + this->endpoint + ", dataType : " + this->dataType + ", dataPollingType : " + this->dataPollingType + ", uid : " + this->uid;
}
