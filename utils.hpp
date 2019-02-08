// utils.hpp
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>

#ifndef UTILS_H
#define UTILS_H

#define ALFRED_SERIAL_SIZE 1500
#define DATASOURCE_SERIAL_SIZE 200
#define ALL_DATASOURCE_SERIAL_SIZE 500

class Request{
  public:
    StaticJsonBuffer<500> jsonBuffer;
    
    Request();
    JsonObject& parseBody();
};

void initWifi(const char * ssid, const char * password);

#endif
