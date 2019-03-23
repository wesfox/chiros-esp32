#include <map>

#include <ArduinoJson.h>
// https://github.com/Makuna/NeoPixelBus/blob/master/examples/NeoPixelTest/NeoPixelTest.ino
#include <NeoPixelBus.h>

#include "Alfred.hpp"

using namespace std;

// https://www.freeformatter.com/json-escape.html

//any modification of this file must be repercuted in the EEPROM
//"{\"name\":\"coolObject\",\"type\":\"lamp\",\"actions\":[{\"name\":\"switch_state\",\"command\":\"/switch\",\"payloads\":[{\"name\":\"switch_state\",\"type\":\"boolean\"}]},{\"name\":\"presence\",\"command\":\"/presence\",\"payloads\":[{\"name\":\"presence\",\"type\":\"boolean\"}]},{\"name\":\"rgb_color\",\"command\":\"/color\",\"payloads\":[{\"name\":\"rgb_color\",\"type\":\"color\"}]}],\"data-source\":[{\"name\":\"switch_state\",\"description\":\" \",\"endpoint\":\"/switch\",\"data-type\":\"boolean\",\"data-polling-type\":\"ON_REQUEST\"},{\"name\":\"rgb_color\",\"description\":\" \",\"endpoint\":\"/color\",\"data-type\":\"color\",\"data-polling-type\":\"ON_REQUEST\"},{\"name\":\"presence\",\"description\":\" \",\"endpoint\":\"/presence\",\"data-type\":\"boolean\",\"data-polling-type\":\"ON_REQUEST\"}]}";
//{
//    "name": "coolObject",
//    "type": "lamp",
//    "actions": [
//        {
//            "name": "switch_state",
//            "command": "/switch",
//            "payloads": [
//                {
//                    "name": "switch_state",
//                    "type": "boolean"
//                }
//            ]
//        },
//        {
//            "name": "presence",
//            "command": "/presence",
//            "payloads": [
//                {
//                    "name": "presence",
//                    "type": "boolean"
//                }
//            ]
//        },
//        {
//            "name": "rgb_color",
//            "command": "/color",
//            "payloads": [
//                {
//                    "name": "rgb_color",
//                    "type": "color"
//                }
//            ]
//        }
//    ],
//    "data-source": [
//        {
//            "name": "switch_state",
//            "description": "",
//            "endpoint": "/switch",
//            "data-type": "boolean",
//            "data-polling-type": "ON_REQUEST"
//        },{
//            "name": "rgb_color",
//            "description": "",
//            "endpoint": "/color",
//            "data-type": "color",
//            "data-polling-type": "ON_REQUEST"
//        },{
//            "name": "presence",
//            "description": "",
//            "endpoint": "/presence",
//            "data-type": "boolean",
//            "data-polling-type": "ON_REQUEST"
//        }
//    ]
//}

//any modification of this file must be repercuted in the EEPROM
//{\"name\":\"rgb_strip\",\"type\":\"rgb_strip\",\"actions\":[{\"name\":\"rgb_color\",\"command\":\"/color\",\"payloads\":[{\"name\":\"rgb_color\",\"type\":\"color\"}]},{\"name\":\"switch_state\",\"command\":\"/switch\",\"payloads\":[{\"name\":\"switch_state\",\"type\":\"boolean\"}]}],\"data-source\":[{\"name\":\"rgb_color\",\"description\":\"desc\",\"endpoint\":\"/color\",\"data-type\":\"color\",\"data-polling-type\":\"ON_REQUEST\"},{\"name\":\"switch_state\",\"description\":\"desc\",\"endpoint\":\"/switch\",\"data-type\":\"boolean\",\"data-polling-type\":\"ON_REQUEST\"}]}
//{
//    "name": "rgb_strip",
//    "type": "rgb_strip",
//    "actions": [
//        {
//            "name": "rgb_color",
//            "command": "/color",
//            "payloads": [
//                {
//                    "name": "rgb_color",
//                    "type": "color"
//                }
//            ]
//        },{
//            "name": "switch_state",
//            "command": "/switch",
//            "payloads": [
//                {
//                    "name": "switch_state",
//                    "type": "boolean"
//                }
//            ]
//        }
//    ],
//    "data-source": [
//        {
//            "name": "rgb_color",
//            "description": "desc",
//            "endpoint": "/color",
//            "data-type": "color",
//            "data-polling-type": "ON_REQUEST"
//        },
//        {
//            "name": "switch_state",
//            "description": "desc",
//            "endpoint": "/switch",
//            "data-type": "boolean",
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
//const char* ssid = "Daniel's iPhone";
//const char* password = "unicornsarecool";

// BVR
// Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, 14, NEO_KHZ800 + NEO_GRB);
const uint16_t PixelCount = 60; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 25;  // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> pixels(PixelCount, PixelPin);

const uint8_t SwitchPin = 17;
const uint8_t DetectPin = 35;

void pinSetup(){
  pinMode(SwitchPin, OUTPUT);
  pinMode(DetectPin, INPUT);
  digitalWrite(SwitchPin, HIGH);
}

RgbColor red(20, 0, 0);
RgbColor green(0, 20, 0);
RgbColor blue(0, 0, 20);

int r=0,g=0,b=0;

void customSetup() {
  pixels.Begin();
  pixels.SetPixelColor(0, red);
  pixels.SetPixelColor(1, green);
  pixels.Show();
  digitalWrite(SwitchPin, alfred.getDataSource("switch_state").serializedState == "true" ? HIGH : LOW);
}

void handleSwitch(){
  // we cannot use HIGH and LOW as we want to make sure to use 0 or 1.
  Serial.println("got into handleSwitch");
  JsonObject& body = request.parseBody();
  int state;
  if(body.size()==0){
    state = digitalRead(SwitchPin) ? 0 : 1;
  }else{
    state = body["payload"].as<boolean>() ? 1 : 0;
  }

  if(state){
    RgbColor payloadColor(r, g, b);
    pixels.ClearTo(payloadColor); // Moderately bright green color.
    pixels.Show();
  }else{
    RgbColor payloadColor(0, 0, 0);
    pixels.ClearTo(payloadColor); // Moderately bright green color.
    pixels.Show();
  }
  digitalWrite(SwitchPin, state);
  alfred.getDataSource("switch_state").serializedState = state ? "true" : "false";
  server.send(200, "text/plain", "Lamped switched " + String(state ? "on" : "off"));
  alfred.sendState("switch_state");
  Serial.println("/switch - Lamped switched " + String(state ? "on" : "off"));
}

void handleColor() {
    Serial.println("got into handleColor");
    JsonObject& body = request.parseBody();
   if(body.size()==0){
    return alfred.handleError(ERROR_ALFRED_NO_PAYLOAD);
   }
   StaticJsonBuffer<300> jsonBuffer;
   JsonObject& payloadObject = jsonBuffer.parseObject(body["payload"].as<String>());
   payloadObject.printTo(Serial);
   r = payloadObject["r"].as<int>();
   g = payloadObject["g"].as<int>();
   b = payloadObject["b"].as<int>();
   Serial.println(String(r) + String(g) + String(b));
//   double total_pow_ratio = double(r+g+b)/100;
//   if(total_pow_ratio>1){
//    r=double(r)/total_pow_ratio;
//    g=double(g)/total_pow_ratio;
//    b=double(b)/total_pow_ratio;
//   }
//   int color=0;
//   int startNumPos=0;
//   for(int i=0;i!=strColor.length();++i){
//      if(strColor[i]=='-'){
//        switch(++color){
//          case 1:
//            r = strColor.substring(startNumPos,i).toInt();
//            break;
//          case 2:
//            v = strColor.substring(startNumPos,i).toInt();
//            break;
//        }
//        startNumPos=i+1;
//      }
//   }
//   if(color!=2){
//      server.send(401, "text/plain", "Error while reading payload");
//   }else{
//      b = strColor.substring(startNumPos,strColor.length()).toInt();
//   }
   
   RgbColor payloadColor(r, g, b);
   pixels.ClearTo(payloadColor); // Moderately bright green color.

   pixels.Show(); // This sends the updated pixel color to the hardware.

   server.send(200, "text/plain", "RGB led strip set to r" + String(r) + " g"+ String(g) + " b" + String(b));
   Serial.println("/setColor - RGB led strip set to r" + String(r) + " g"+ String(g) + " b" + String(b));

   String serializedState;
   body["payload"].printTo(serializedState);
    alfred.getDataSource("rgb_color").serializedState = serializedState;
    Serial.println(serializedState);
    alfred.sendState("rgb_color");
}

const unsigned long timeUntilStop = 10000;//30 * 60 * 1000;

unsigned long timeLastDectect = 0;
unsigned long timeLastNotDectect = 0;
int durationDetection = 0;
boolean movementDectected = false;
boolean personDectected = false;
void customLoop(){
  movementDectected = digitalRead(DetectPin);
  if(movementDectected){
    timeLastDectect = millis();
    delay(1);
    if(!personDectected){
      alfred.getDataSource("presence").serializedState = "true";
      personDectected = true;
      alfred.sendState("presence");
      Serial.println("You have been detected");
    }
  }
  if(personDectected && abs(millis()-timeLastDectect) > timeUntilStop){
    Serial.println(abs(millis()-timeLastDectect) - timeUntilStop);
    personDectected = false;
    alfred.getDataSource("presence").serializedState = "false";
    alfred.sendState("presence");
    Serial.println("You do not exist anymore");
  }
}

// custom routes
void initCustomRoutes(){

  // you may use the extractPostedPayload function to retrieve posted data
  server.on("/switch", handleSwitch);
  server.on("/color", handleColor);

  Serial.println("configured custom routes");

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
  alfred.alfredSetup(ssid, password);
  pinSetup();
  customSetup();
  initCustomRoutes();
  if(alfred.testIsAlfredUp()){
    alfred.initialized = true;
  }
}

/////////
// LOOP
void loop(void) {
  server.handleClient();
  if(alfred.initialized){
    customLoop();
    alfred.checkForDataToPush();
  }
}
