#include "ApplicationSettings.h"
#include "application.h"

ApplicationSettings settings;

const char DEFAULT_ADDRESS[OSC_MESSAGE_COUNT][OSC_ADDRESS_MAX_LEN+1] =
  { "/status", 
    "/a/cv", 
    "/b/cv", 
    "/a/tr", 
    "/b/tr" };

#define UDP_REMOTE_PORT       9000
#define UDP_LOCAL_PORT        8000

void ApplicationSettings::reset(){
  strncpy(inputPrefix, "/osm", sizeof(inputPrefix));
  strncpy(outputPrefix, "/osm", sizeof(outputPrefix));
  remotePort = UDP_REMOTE_PORT;
  localPort = UDP_LOCAL_PORT;
  autoremote = true;
  broadcast = true;
  memcpy(inputAddress, DEFAULT_ADDRESS, sizeof(DEFAULT_ADDRESS));
  memcpy(outputAddress, DEFAULT_ADDRESS, sizeof(DEFAULT_ADDRESS));
  for(int i=0; i<4; ++i){
    min[i] = -1;
    max[i] = 1;
  }
}
