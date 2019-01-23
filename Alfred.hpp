// Alfred.hpp

#include "DataSource.hpp"

#ifndef ALFRED_H
#define ALFRED_H

class Alfred {
   public:
      Alfred();
      Alfred(const char* url, const char* uid, int port, JsonObject& dataSourceIds);

      void saveToEEPROM();
      void loadFromEEPROM();
      
      void showConf();
      void sendState(String sourceName);
      
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

#endif
