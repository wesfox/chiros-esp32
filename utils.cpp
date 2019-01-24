// utils.cpp

#include <ArduinoJson.h>

#include "utils.hpp"
#include "Alfred.hpp"

Request::Request(){}

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

void initWifi(const char * ssid, const char * password){
 Serial.println("Initializing wifi connection with:");
 Serial.print("SSID: ");Serial.println(ssid);
 Serial.print("Password: ");Serial.println(password);
 WiFi.mode(WIFI_STA);
 WiFi.begin(ssid, password);

 // Wait for connection
 while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
 }
 Serial.println("");
 Serial.print("Connected with ip adress: ");
 Serial.println(WiFi.localIP());
}
