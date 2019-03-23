// utils.hpp
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>

#ifndef UTILS_H
#define UTILS_H

#define ALFRED_SERIAL_SIZE 2000
#define DATASOURCE_SERIAL_SIZE 500
#define ALL_DATASOURCE_SERIAL_SIZE 1700

class Request{
  public:
    StaticJsonBuffer<2000> jsonBuffer;
    
    Request();
    JsonObject& parseBody();
};

void initWifi(const char * ssid, const char * password);

#endif
