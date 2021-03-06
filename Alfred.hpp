// Alfred.hpp

#include "DataSource.hpp"
#include "utils.hpp"
#include <Preferences.h>

#ifndef ALFRED_H
#define ALFRED_H

#define ERROR_ALFRED_NO_PAYLOAD 1

//typedef void (*pinSetupFunc)();
//typedef void (*customSetupFunc)();
//typedef void (*initCustomRoutesFunc)();

class Alfred {
   public:
      Alfred();
      
      void init(const char* url, const char* uid, int port, JsonObject& dataSourceIds, String objConfig);

      void alfredSetup(const char * ssid, const char * password);
      void handleError(int errorDef);

      void saveToEEPROM();
      void loadFromEEPROM();
      
      void showConf();
      void sendState(String sourceName);
      boolean testIsAlfredUp();
      
      DataSource& getDataSource(String dataSourceName);
      DataSource& getDataSourceById(String dataSourceUid);
      void checkForDataToPush();
      void setTimeBetweenPushForDS(unsigned long timeBetweenPush, String dataSourceName);
      
      int port;
      String url;
      String uid;
      
      DataSource* dataSourceList;
      int nbDataSources;
      
      bool initialized;
};

extern Alfred alfred;
extern String DefaultObjConfig;
extern Preferences preferences;
extern Request request;
extern WebServer server;

#endif
