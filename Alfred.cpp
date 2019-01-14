#include <HTTPClient.h>

#include "utils.hpp"
#include "Alfred.hpp"
#include "DataSource.hpp"

// Alfred.cpp

Alfred::Alfred() {
  this->initialized = false;
}

Alfred::Alfred(const char* url, const char* uid, int port, JsonObject& dataSourceIds) {
  StaticJsonBuffer<4000> jsonBuffer;

  JsonObject& jsonConfig = jsonBuffer.parseObject(objConfig);
  JsonArray& dataSources = jsonConfig["data-source"];

  this->dataSourceList = new DataSource[dataSources.size()];
  this->nbDataSources = dataSources.size();

  for(int i=0; i!=dataSources.size(); i++){
    this->dataSourceList[i] = DataSource(
       dataSources[i]["name"],
       dataSources[i]["description"],
       dataSources[i]["endpoint"],
       dataSources[i]["data-type"],
       dataSources[i]["data-polling-type"]
    );
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
}

void Alfred::sendState(){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  // cast value if possible
  if(this->dataSourceList[0].dataType == "boolean")
    payload["value"] = (boolean)(this->dataSourceList[0].serializedState == "true" ? true : false);

  // add uid
  payload["data_source_id"] = this->dataSourceList[0].uid;

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
