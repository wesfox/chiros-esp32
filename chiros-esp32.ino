#include <map>

#include <ArduinoJson.h>
// https://github.com/Makuna/NeoPixelBus/blob/master/examples/NeoPixelTest/NeoPixelTest.ino
#include <NeoPixelBus.h>
#include <Preferences.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "EEPROM.h"

#include "Alfred.hpp"

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
//            "name": "rgb_color",
//            "command": "/setColor",
//            "payloads": [
//                {
//                    "name": "rgb_color",
//                    "type": "string"
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
//        },{
//            "name": "rgb_color",
//            "description": "return the current strip color",
//            "endpoint": "/setColor",
//            "data-type": "string",
//            "data-polling-type": "ON_REQUEST"
//        }
//    ]
//}

Alfred alfred;
WebServer server(80);

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// DO NOT MODIFY ANYTHING ABOVE THIS LINE ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////
///

const char* ssid = "Dinosorus";
const char* password = "LaRicanerieDeDinosorus";
//const char* ssid = "{VIA}";
//const char* password = "connexion";
//const char* ssid = "Livebox-BABA";
//const char* password = "HRZxJmJyUjWPb7Cymw";

// BVR
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 14, NEO_KHZ800 + NEO_GRB);
const uint16_t PixelCount = 2; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 25;  // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(PixelCount, PixelPin);

const uint8_t SwitchPin = 17;

void pinSetup(){
  pinMode(SwitchPin, OUTPUT);
  digitalWrite(SwitchPin, LOW);
}

RgbColor red(20, 0, 0);
RgbColor green(0, 20, 0);
RgbColor blue(0, 0, 20);

void customSetup() {
  pixels.Begin();
  pixels.SetPixelColor(0, red);
  pixels.SetPixelColor(1, green);
  pixels.Show();
}

void handleSwitch(){
  // we cannot use HIGH and LOW as we want to make sure to use 0 or 1.
  int state = digitalRead(SwitchPin) ? 0 : 1;
  digitalWrite(SwitchPin, state);
  alfred.getDataSource("state").serializedState = state ? "true" : "false";
  server.send(200, "text/plain", "Lamped switched " + String(state ? "on" : "off"));
  alfred.sendState("state");
  Serial.println("/switch - Lamped switched " + String(state ? "on" : "off"));
}

void handleColor() {
    JsonObject& body = request.parseBody();
   if(body.size()==0){
    return alfred.handleError(ERROR_ALFRED_NO_PAYLOAD);
   }
   
   String strColor = body["payload"];
   int r,v,b;
   int color=0;
   int startNumPos=0;
   for(int i=0;i!=strColor.length();++i){
      if(strColor[i]=='-'){
        switch(++color){
          case 1:
            r = strColor.substring(startNumPos,i).toInt();
            break;
          case 2:
            v = strColor.substring(startNumPos,i).toInt();
            break;
        }
        startNumPos=i+1;
      }
   }
   if(color!=2){
      server.send(401, "text/plain", "Error while reading payload");
   }else{
      b = strColor.substring(startNumPos,strColor.length()).toInt();
   }
   RgbColor payloadColor(r, v, b);
   pixels.ClearTo(payloadColor); // Moderately bright green color.

   pixels.Show(); // This sends the updated pixel color to the hardware.

   server.send(200, "text/plain", "RGB led strip set to " + strColor);
   Serial.println("/setColor - RGB led strip set to " + strColor);
   
    alfred.getDataSource("rgb_color").serializedState = strColor;
    alfred.sendState("rgb_color");
}

// custom routes
void initCustomRoutes(){

  // you may use the extractPostedPayload function to retrieve posted data
  
  server.on("/switch", handleSwitch);
  server.on("/on", []() {
    digitalWrite(SwitchPin, HIGH);
    alfred.getDataSource("state").serializedState = "true";
    alfred.sendState("state");
    server.send(200, "text/plain", "Lamped switched on");
    Serial.println("/on - Lamped switched on");
  });
  server.on("/off", []() {
    digitalWrite(SwitchPin, LOW);
    alfred.getDataSource("state").serializedState = "false";
    alfred.sendState("state");
    server.send(200, "text/plain", "Lamped switched off");
    Serial.println("/on - Lamped switched off");
  });

  server.on("/setColor", handleColor);

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
// SETUP

void setup(void) {
  Serial.begin(115200);
  alfred.alfredSetup(pinSetup, customSetup, initCustomRoutes, ssid, password);
}

/////////
// LOOP
void loop(void) {
  server.handleClient();
  alfred.checkForDataToPush();
}
