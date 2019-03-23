#include "Alfred.hpp"

using namespace std;

Alfred alfred;
WebServer server(80);

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// DO NOT MODIFY ANYTHING ABOVE THIS LINE ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////
///

const char* ssid = "";
const char* password = "";

const int readPin = 10;
const int syncedPin = 11;


void pinSetup(){
  pinMode(readPin, INPUT);
  pinMode(syncedPin, OUTPUT);
}

void customSetup() {
  // insert your setup() (serial.begin(115200) is done by default)
}

void handleExemple(){
  // if this function is called 11 is synced with 10
  digitalWrite(syncedPin, alfred.getDataSource("state").serializedState == "true" ? HIGH : LOW);
}

int lastState = digitalRead(readPin);
void customLoop(){
  // define your own loop
  int newState = digitalRead(readPin);
  if(newState != lastState){
    alfred.getDataSource("state").serializedState = newState ? "true" : "false";
    alfred.sendStatte("state");
    lastState = newState;
    Serial.println("New state detected : " + String(newState));
  }
}

// custom routes
void initCustomRoutes(){

  // you may use the extractPostedPayload function to retrieve posted data
  // server.on("/switch", handleSwitch);
  server.on("/exemple", handleExemple);

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
}

/////////
// LOOP
void loop(void) {
  server.handleClient();
  customLoop();
  alfred.checkForDataToPush();
}
