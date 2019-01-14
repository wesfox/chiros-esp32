// DataSource.hpp

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

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

#endif