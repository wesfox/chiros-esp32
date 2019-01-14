// utils.cpp

#include <ArduinoJson.h>

#include "utils.hpp"

const char* objConfig = "{\n\"name\":\"coolObject\",\n\"type\":\"lamp\",\n\"actions\":[\n{\n\"name\":\"switch\",\n\"command\":\"\/switch\",\n\"payloads\":[\n{\n\"name\":\"switch\",\n\"type\":\"boolean\"\n}\n]\n},\n{\n\"name\":\"on\",\n\"command\":\"\/on\"\n},\n{\n\"name\":\"off\",\n\"command\":\"\/off\"\n}\n],\n\"data-source\":[\n{\n\"name\":\"state\",\n\"description\":\"return the state\",\n\"endpoint\":\"\/state\",\n\"data-type\":\"boolean\",\n\"data-polling-type\":\"ON_REQUEST\"\n}\n]\n}";
WebServer server(80);
Alfred alfred = Alfred();

JsonObject& extractPostedPayload(){
  StaticJsonBuffer<2000> jsonBuffer;

  // reset JsonObject
  JsonObject& root = jsonBuffer.createObject();

  if(server.hasArg("plain")){
  JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
    return root;
  }else{
    return root;
  }
}
