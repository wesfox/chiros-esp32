// utils.hpp
#include <ArduinoJson.h>
#include <WebServer.h>

#include "Alfred.hpp"

#ifndef UTILS_H
#define UTILS_H

JsonObject& extractPostedPayload();

// max size is >~ 3000 caracters
extern const char* objConfig;
extern WebServer server;
extern Alfred alfred;

#endif
