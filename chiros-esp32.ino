#include <map>

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#include "Alfred.hpp"
#include "DataSource.hpp"
#include "routeHandlers.hpp"
#include "utils.hpp"

using namespace std;

// https://www.freeformatter.com/json-escape.html

//any modification of this file must be repercuted in the EEPROM
//{
//    "name": "coolObject",
//    "type": "lamp",
//    "actions": [
//        {
//            "name": "switch",
//            "command": "/switch",
//            "payloads": [
//                {
//                    "name": "switch",
//                    "type": "boolean"
//                }
//            ]
//        },
//        {
//            "name": "on",
//            "command": "/on"
//        },
//        {
//            "name": "off",
//            "command": "/off"
//        }
//    ],
//    "data-source": [
//        {
//            "name": "state",
//            "description": "return the state",
//            "endpoint": "/state",
//            "data-type": "boolean",
//            "data-polling-type": "ON_REQUEST"
//        }
//    ]
//}

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// DO NOT MODIFY ANYTHING ABOVE THIS LINE ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////
///

const char* ssid = "Dinosorus";
const char* password = "LaRicanerieDeDinosorus";

void pinSetup(){
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
}

void handleSwitch(){
  Serial.println("/switch reached.");
  // we cannot use HIGH and LOW as we want to make sure to use 0 or 1.
  int state = digitalRead(4) ? 0 : 1;
  digitalWrite(4, state);
  alfred.getDataSource("state").serializedState = state ? "true" : "false";
  server.send(200, "text/plain", "switched");
  alfred.sendState();
}

// custom routes
void initCustomRoutes(){

  // you may use the extractPostedPayload function to retrieve posted data
  
  server.on("/switch", handleSwitch);
  server.on("/on", []() {
    digitalWrite(4, HIGH);
  });
  server.on("/off", []() {
    digitalWrite(4, LOW);
  });

  ///////
  // basic routing style
  ///////
  // server.on("/my_route", handleRouteFunction);
  
  ///////
  // inline routing style
  ///////
  //
  // server.on("/inline", []() {
  //  // ...todo
  // });
}

///
/////////////////
//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// DO NOT MODIFY ANYTHING UNDER THIS LINE ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

/////////
// init

void initWifi(){
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

void initRouting(){
  server.on("/", handleRoot);

  // default routes
  server.on("/config", handleConfig);
  server.on("/serverConfig", handleSetConfig);

  initCustomRoutes();
}

/////////
// SETUP

void setup(void) {
 Serial.begin(115200);
 pinSetup();

 // conntect to the wifi network as
 // specified on top of the program
 initWifi();

 if (MDNS.begin("esp32")) {
  Serial.println("MDNS responder started");
 }

 // define all routes callable
 initRouting();

 // raise an error if another route is called
 server.onNotFound(handleNotFound);

 server.begin();
 Serial.println("HTTP server started");
}

/////////
// LOOP

void loop(void) {
  server.handleClient();
}
