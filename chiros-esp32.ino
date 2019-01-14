#include <map>

#include <ArduinoJson.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

using namespace std;

// https://www.freeformatter.com/json-escape.html

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

// max size is >~ 3000 caracters
const char* objConfig = "{\n\"name\":\"coolObject\",\n\"type\":\"lamp\",\n\"actions\":[\n{\n\"name\":\"switch\",\n\"command\":\"\/switch\",\n\"payloads\":[\n{\n\"name\":\"switch\",\n\"type\":\"boolean\"\n}\n]\n},\n{\n\"name\":\"on\",\n\"command\":\"\/on\"\n},\n{\n\"name\":\"off\",\n\"command\":\"\/off\"\n}\n],\n\"data-source\":[\n{\n\"name\":\"state\",\n\"description\":\"return the state\",\n\"endpoint\":\"\/state\",\n\"data-type\":\"boolean\",\n\"data-polling-type\":\"ON_REQUEST\"\n}\n]\n}";

//////////////////////////////////////////////////////////////////////////////////////////
////////////////////// DO NOT MODIFY ANYTHING UNDER THIS LINE ////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

WebServer server(80);

class DataSource {
  public:
      DataSource();
      DataSource(const char* name, const char* description, const char* endpoint, const char* dataType, const char* dataPollingType);
      String toString();
      
      String name;
      String description;
      String endpoint;
      String dataType;
      String dataPollingType;
      String serializedState;

      String uid;
};
DataSource::DataSource(){}

DataSource::DataSource(
  const char* name, 
  const char* description,
  const char* endpoint,
  const char* dataType,
  const char* dataPollingType){
    this->name = name;
    this->description = description;
    this->endpoint = endpoint;
    this->dataType = dataType;
    this->dataPollingType = dataPollingType;
}

String DataSource::toString(){
  return "name : " + this->name + ", serializedState : " + this->serializedState + ", description : " + this->description + ", endpoint : " + this->endpoint + ", dataType : " + this->dataType + ", dataPollingType : " + this->dataPollingType + ", uid : " + this->uid;
}

////////////////////

class Alfred {
   public:
      Alfred();
      Alfred(const char* url, const char* uid, int port, JsonObject& dataSourceIds);
      
      void showConf();
      void sendState();
      
      DataSource& getDataSource(String dataSourceName);
      
      int port;
      String url;
      String uid;
      
      DataSource* dataSourceList;
      int nbDataSources;
      
      bool initialized;
};

Alfred::Alfred() {
  this->initialized = false;
}

Alfred::Alfred(const char* url, const char* uid, int port, JsonObject& dataSourceIds) {
  StaticJsonBuffer<4000> jsonBuffer;

  JsonObject& jsonConfig = jsonBuffer.parseObject(objConfig);
  JsonArray& dataSources = jsonConfig["data-source"];

  this->dataSourceList = new DataSource[dataSources.size()];
  this->nbDataSources = dataSources.size();

  for(int i=0; i!=dataSources.size(); i++){
    this->dataSourceList[i] = DataSource(
       dataSources[i]["name"],
       dataSources[i]["description"],
       dataSources[i]["endpoint"],
       dataSources[i]["data-type"],
       dataSources[i]["data-polling-type"]
    );
    for (JsonObject::iterator it=dataSourceIds.begin(); it!=dataSourceIds.end(); ++it) {
      if(String(it->key).equals(this->dataSourceList[i].name)){
        this->dataSourceList[i].uid = it->value.asString();
      }
    }
  }

  this->url = url;
  this->port = port;
  this->uid = uid;
  
  this->initialized = true;
}

void Alfred::sendState(){
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& payload = jsonBuffer.createObject();

  // cast value if possible
  if(this->dataSourceList[0].dataType == "boolean")
    payload["value"] = (boolean)(this->dataSourceList[0].serializedState == "true" ? true : false);

  // add uid
  payload["data_source_id"] = this->dataSourceList[0].uid;

  // serialize
  String strPayload;
  payload.printTo(strPayload);

  // send the result to alfred-brain
  HTTPClient http;
  http.begin("http://" + this->url + ":" + String(this->port) + "/saveDataPoint");
  http.addHeader("Content-Type", "application/json");
  Serial.println("Send : " + strPayload);
  Serial.println("To : http://" + this->url + ":" + String(this->port) + "/saveDataPoint");
  int httpResponseCode = http.POST(strPayload);
}

DataSource& Alfred::getDataSource(String dataSourceName){
  for(int i=0; i!=this->nbDataSources; i++){
    if(dataSourceName.equals(this->dataSourceList[i].name)){
      return this->dataSourceList[i];
    }
  }
  Serial.println("Error, unkown dataSourceName : " + dataSourceName + " used");
  server.send(500, "Internal Error : unkown dataSourceName");
}

void Alfred::showConf() {
  Serial.print("url : ");
  Serial.println(this->url);
  
  Serial.print("port : ");
  Serial.println(this->port);
  
  Serial.print("uid : ");
  Serial.println(this->uid);
  
  Serial.println("dataSources : ");
  for(int i=0; i!=this->nbDataSources; i++){
    Serial.println(this->dataSourceList[i].toString());
  }
}

Alfred alfred = Alfred();

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
// utils

JsonObject& extractPostedPayload(){
 
 StaticJsonBuffer<2000> jsonBuffer;
 
 // reset JsonObject
 JsonObject& root = jsonBuffer.parseObject("");
 
 if(server.hasArg("plain")){
  JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));
  return root;
 }else{
  return root;
 }
}

/////////
// routesHandler

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

void handleErrorNoPayload(){
 server.send(401, "text/plain", "Error, no payload found in the request.");
}

void handleRoot() {
 server.send(200, "text/plain", "/register is available");
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

void extractConfig(){
  StaticJsonBuffer<2000> jsonBuffer2;
  JsonObject& jsonConfig = jsonBuffer2.parseObject(objConfig);
   const char* myname = jsonConfig["name"];
   Serial.println(myname);
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

 // extract the config of the object
 extractConfig();

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
