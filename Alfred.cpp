#include <HTTPClient.h>

#include "utils.hpp"
#include "Alfred.hpp"
#include "DataSource.hpp"

// Alfred.cpp

Alfred::Alfred() {
  this->initialized = false;  
}

Alfred::Alfred(const char* url, const char* uid, int port, JsonObject& dataSourceIds) {
  StaticJsonBuffer<ALFRED_SERIAL_SIZE> jsonBuffer;

  JsonObject& jsonConfig = jsonBuffer.parseObject(objConfig);
  JsonArray& dataSources = jsonConfig["data-source"];

  this->dataSourceList = new DataSource[dataSources.size()];
  this->nbDataSources = dataSources.size();

  Serial.println(url);
  Serial.println(port);
  Serial.println(uid);
  
//  Serial.println("dataSourceList malloced");

  for(int i=0; i!=dataSources.size(); i++){
    this->dataSourceList[i] = DataSource(
       dataSources[i]["name"],
       dataSources[i]["description"],
       dataSources[i]["endpoint"],
       dataSources[i]["data-type"],
       dataSources[i]["data-polling-type"]
    );
//    Serial.println("dataSourceList populated");
    for (JsonObject::iterator it=dataSourceIds.begin(); it!=dataSourceIds.end(); ++it) {
      if(String(it->key).equals(this->dataSourceList[i].name)){
        this->dataSourceList[i].uid = it->value.asString();
      }
    }
  }

  this->url = url;
  this->port = port;
  this->uid = uid;
  
  this->initialized = true;

  this->showConf();

  this->saveToEEPROM();
}

void Alfred::saveToEEPROM(){
  Serial.println("creating jsonBuffer");
  StaticJsonBuffer<ALFRED_SERIAL_SIZE> jsonBuffer;
  Serial.println("creating root");
  JsonObject& root = jsonBuffer.createObject();
  Serial.println("done");

  root["url"] = this->url;
  root["port"] = this->port;
  root["uid"] = this->uid;
  root["nbDataSources"] = this->nbDataSources;

  JsonArray& dataSourceList = root.createNestedArray("dataSourceList");
  StaticJsonBuffer<DATASOURCE_SERIAL_SIZE> jsonBufferDS;
  StaticJsonBuffer<ALL_DATASOURCE_SERIAL_SIZE> jsonBufferExportDS;
  for(int i=0; i!=this->nbDataSources; i++){
    JsonObject& dataSource = jsonBufferDS.createObject();
    dataSource["name"] = this->dataSourceList[i].name;
    dataSource["description"] = this->dataSourceList[i].description;
    dataSource["endpoint"] = this->dataSourceList[i].endpoint;
    dataSource["serializedState"] = this->dataSourceList[i].serializedState;
    dataSource["dataType"] = this->dataSourceList[i].dataType;
    dataSource["dataPollingType"] = this->dataSourceList[i].dataPollingType;
    dataSource["uid"] = this->dataSourceList[i].uid;
    
    String serializedDataSource;
    dataSource.printTo(serializedDataSource);
    dataSourceList.add(jsonBufferExportDS.parseObject(serializedDataSource));
    jsonBufferDS.clear();
  }
  String serializedJson;
  root.printTo(serializedJson);
  preferences.begin("alfred", false);
  preferences.putString("alfredSer", serializedJson);
  preferences.end();
  Serial.println("Alfred Serialized : "+serializedJson);
}

void Alfred::loadFromEEPROM(){
  preferences.begin("alfred", false);
  String alfredSerialized = preferences.getString("alfredSer", "undefined");
  preferences.end();
  if(alfredSerialized.equals("undefined")){
    // alfred have never been serialized, we may not continue
    Serial.println("No alfred serialized dump found.");
    return;
  }
  Serial.println("Alfred serialized dump found : "+alfredSerialized);
  
  StaticJsonBuffer<ALFRED_SERIAL_SIZE> jsonBuffer;
  StaticJsonBuffer<DATASOURCE_SERIAL_SIZE> dataSourceBuffer;
  JsonObject& alfredObject = jsonBuffer.parseObject(alfredSerialized);
  
  this->url = alfredObject.get<String>("url");
  this->port = alfredObject["port"];
  this->uid = alfredObject.get<String>("uid");
  
  this->nbDataSources = alfredObject["nbDataSources"];
  this->dataSourceList = new DataSource[this->nbDataSources];
  
  for(int i=0; i!=this->nbDataSources; i++){
    String serializedDataSource;
    alfredObject["dataSourceList"][i].printTo(serializedDataSource);
    JsonObject& dataSource = jsonBuffer.parseObject(serializedDataSource);
    this->dataSourceList[i] = DataSource(
      dataSource["name"],
      dataSource["description"],
      dataSource["endpoint"],
      dataSource["dataType"],
      dataSource["dataPollingType"]
    );
    this->dataSourceList[i].uid = dataSource.get<String>("uid");
    this->dataSourceList[i].serializedState = dataSource.get<String>("serializedState");
  }

  this->showConf();
}

void Alfred::sendState(String sourceName){
  StaticJsonBuffer<DATASOURCE_SERIAL_SIZE> jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  DataSource dataSource = this->getDataSource(sourceName);

  // cast value if possible
  if(dataSource.dataType == "boolean")
    payload["value"] = (boolean)(dataSource.serializedState == "true" ? true : false);
  else
    payload["value"] = dataSource.serializedState;

  // add uid
  payload["data_source_id"] = dataSource.uid;

  // serialize
  String strPayload;
  payload.printTo(strPayload);

  // send the result to alfred-brain
  HTTPClient http;
  http.begin("http://" + this->url + ":" + String(this->port) + "/saveDataPoint");
  http.addHeader("Content-Type", "application/json");
  Serial.println("Send : " + strPayload);
  Serial.println("To : http://" + this->url + ":" + String(this->port) + "/saveDataPoint");
  int httpResponseCode = http.POST(strPayload);
  Serial.println("Http response code : "+String(httpResponseCode));
  http.end();
}

void Alfred::checkForDataToPush(){
  unsigned long alfredTime = millis(); 
  for(int i=0; i!=this->nbDataSources; i++){
    // abs allow us to make sure that even if there is an overflow, it will only send the data twice in a range.
    if(
      this->dataSourceList[i].timeBetweenPush
      && (abs(alfredTime - this->dataSourceList[i].lastPush) > this->dataSourceList[i].timeBetweenPush)
    ){
      this->dataSourceList[i].lastPush = alfredTime;
      this->sendState(this->dataSourceList[i].name);
    }
  }
}

void Alfred::setTimeBetweenPushForDS(unsigned long timeBetweenPush, String dataSourceUid){
  DataSource dataSource = this->getDataSourceById(dataSourceUid);

  Serial.println("Set time for "+dataSource.name+" to : "+String(timeBetweenPush));
  dataSource.setPush(timeBetweenPush);
}

DataSource& Alfred::getDataSourceById(String dataSourceUid){
  for(int i=0; i!=this->nbDataSources; i++){
    if(dataSourceUid.equals(this->dataSourceList[i].uid)){
      return this->dataSourceList[i];
    }
  }
  Serial.println("Error, unkown dataSourceId : " + dataSourceUid + " used");
  server.send(500, "Internal Error : unkown dataSourceName");
}

DataSource& Alfred::getDataSource(String dataSourceName){
  for(int i=0; i!=this->nbDataSources; i++){
    if(dataSourceName.equals(this->dataSourceList[i].name)){
      return this->dataSourceList[i];
    }
  }
  Serial.println("Error, unkown dataSourceName : " + dataSourceName + " used");
  server.send(500, "Internal Error : unkown dataSourceName");
}

void Alfred::showConf() {
  Serial.print("url : ");
  Serial.println(this->url);
  
  Serial.print("port : ");
  Serial.println(this->port);
  
  Serial.print("uid : ");
  Serial.println(this->uid);
  
  Serial.println("dataSources : ");
  for(int i=0; i!=this->nbDataSources; i++){
    Serial.println(this->dataSourceList[i].toString());
  }
}
