// routeHandlers.cpp

#include <WebServer.h>
#include <ArduinoJson.h>

#include "utils.hpp"
#include "Alfred.hpp"
#include "routeHandlers.hpp"

void handleConfig() {
   server.send(200, "text/json", objConfig);
}

void handleSetConfig(){
   JsonObject& registerPayload = extractPostedPayload();
   if(registerPayload.size()==0){
    return handleErrorNoPayload();
   }
   alfred = Alfred(registerPayload["ip"], registerPayload["uid"], registerPayload["port"], registerPayload["data-source-ids"]);
   alfred.showConf();
   server.send(200, "text/plain", "Server config has been set");
}

void handleRoot() {
   server.send(200, "text/plain", "/register is available");
}

void handleErrorNoPayload(){
   server.send(401, "text/plain", "Error, no payload found in the request.");
}

void handleNotFound() {
   String message = "Url Not Found\n\n";
   message += "URI: ";
   message += server.uri();
   message += "\nMethod: ";
   message += (server.method() == HTTP_GET) ? "GET" : "POST";
   message += "\nArguments: ";
   message += server.args();
   message += "\n";
   for (uint8_t i = 0; i < server.args(); i++) {
   message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
   }
   server.send(404, "text/plain", message);
}
