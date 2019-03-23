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

void pinSetup(){
  // insert your
}

void customSetup() {
  // insert your setup() (serial.begin(115200) is done by default)
}

void handleExemple(){
  Serial.println("/exemple was called");
}

void customLoop(){
  // define your own loop
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
