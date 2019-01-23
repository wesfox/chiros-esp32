// routeHandlers.hpp

#ifndef ROOT_HANDLERS_H
#define ROOT_HANDLERS_H

// routes
void handleConfig();
void handleSetConfig();
void handleRoot();
void handleSetPush();

// errors
void handleNotFound();
void handleErrorNoPayload();

#endif
