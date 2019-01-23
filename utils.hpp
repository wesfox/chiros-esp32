// utils.hpp
#include <ArduinoJson.h>
#include <WebServer.h>
#include <Preferences.h>


#include "Alfred.hpp"

#ifndef UTILS_H
#define UTILS_H

#define ALFRED_SERIAL_SIZE 2000
#define DATASOURCE_SERIAL_SIZE 300
#define ALL_DATASOURCE_SERIAL_SIZE 1000

class Request{
  public:
    StaticJsonBuffer<1000> jsonBuffer;
    
    Request();
    JsonObject& parseBody();
};

extern const char* objConfig;
extern WebServer server;
extern Alfred alfred;
extern Preferences preferences;
extern Request request;

#endif
