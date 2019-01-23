// utils.cpp

#include <ArduinoJson.h>

#include "utils.hpp"

const char* objConfig = "{\n \"name\": \"coolObject\",\n \"type\": \"lamp\",\n \"actions\": [\n {\n \"name\": \"switch\",\n \"command\": \"\/switch\",\n \"payloads\": [\n {\n \"name\": \"switch\",\n \"type\": \"boolean\"\n }\n ]\n },\n {\n \"name\": \"rgb_color\",\n \"command\": \"\/setColor\",\n \"payloads\": [\n {\n \"name\": \"rgb_color\",\n \"type\": \"string\"\n }\n ]\n },\n {\n \"name\": \"on\",\n \"command\": \"\/on\"\n },\n {\n \"name\": \"off\",\n \"command\": \"\/off\"\n }\n ],\n \"data-source\": [\n {\n \"name\": \"state\",\n \"description\": \"return the state\",\n \"endpoint\": \"\/state\",\n \"data-type\": \"boolean\",\n \"data-polling-type\": \"ON_REQUEST\"\n },{\n \"name\": \"rgb_color\",\n \"description\": \"return the current strip color\",\n \"endpoint\": \"\/setColor\",\n \"data-type\": \"string\",\n \"data-polling-type\": \"ON_REQUEST\"\n }\n ]\n}";
WebServer server(80);
Alfred alfred = Alfred();
Preferences preferences;
Request request;

Request::Request(){
  
}

JsonObject& Request::parseBody(){
  // reset JsonObject
  this->jsonBuffer.clear();
  jsonBuffer.parseObject(server.arg("plain")).printTo(Serial);
  if(server.hasArg("plain")){
    return this->jsonBuffer.parseObject(server.arg("plain"));
  }else{
    return this->jsonBuffer.createObject();
  }
}
