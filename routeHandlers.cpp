// routeHandlers.cpp

#include <WebServer.h>
#include <ArduinoJson.h>

#include "utils.hpp"
#include "Alfred.hpp"
#include "routeHandlers.hpp"

void handleConfig() {
   server.send(200, "text/json", objConfig);
}

void handleSetPush(){
   JsonObject& body = request.parseBody();
   if(body.size()==0){
    return handleErrorNoPayload();
   }

   alfred.setTimeBetweenPushForDS(body["time"], body["dataSourceId"]);
   server.send(200, "text/plain", "Server config has been set");
}

void handleSetConfig(){
//   {
//    "ip":"192.168.1.43",
//    "uid":"sfads-qwe-sddqwe",
//    "port":8000,
//    "data-source-ids": {
//      "name1":"131fs-safd23-sfsfd1",
//      "name2":"qwe-dfs-123",
//      ...
//    }
//   }
   JsonObject& registerPayload = request.parseBody();
   if(registerPayload.size()==0){
    return handleErrorNoPayload();
   }
   // {"url": "127.0.0.1", "port": "8000", "id": "dc031f15-a3cf-4a75-8c9e-6e02e7ff80e7", "data-source-ids": {"state": "99e5e872-a9e4-4d22-806a-45e1bf54fedc"}}
   Serial.println("registerPayload : ");
   registerPayload.printTo(Serial);
   alfred = Alfred(registerPayload["url"], registerPayload["id"], registerPayload["port"], registerPayload["data-source-ids"]);
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
