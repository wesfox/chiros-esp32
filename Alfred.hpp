// Alfred.hpp

#include "DataSource.hpp"

#ifndef ALFRED_H
#define ALFRED_H

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

#endif
