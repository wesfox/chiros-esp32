// DataSource.hpp

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

#define DEFAULT_TIME_BETWEEN_PUSH 60000

class DataSource {
  public:
      DataSource();
      DataSource(const char* name, const char* description, const char* endpoint, const char* dataType, const char* dataPollingType);
      String toString();

      void stopPush();
      void startPush();
      void setPush(unsigned long timeBetweenPush);
      
      String name;
      String description;
      String endpoint;
      String dataType;
      String dataPollingType;
      String serializedState;

      unsigned long timeBetweenPush;
      unsigned long lastPush;

      String uid;
};

#endif
