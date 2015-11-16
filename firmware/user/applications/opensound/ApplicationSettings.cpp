#include "ApplicationSettings.h"
#include "application.h"

NetworkSettings networkSettings;
AddressSettings addressSettings;
RangeSettings rangeSettings;

const char DEFAULT_ADDRESS[5][16] = { "/osm/status", "/osm/a/cv", "/osm/b/cv", "/osm/a/tr", "/osm/b/tr" };
#define UDP_REMOTE_PORT       9000
#define UDP_LOCAL_PORT        8000

void NetworkSettings::reset(){
  remotePort = UDP_REMOTE_PORT;
  localPort = UDP_LOCAL_PORT;
  autoremote = true;
  broadcast = true;
}

void AddressSettings::reset(){
  memcpy(inputAddress, DEFAULT_ADDRESS, sizeof(DEFAULT_ADDRESS));
  memcpy(outputAddress, DEFAULT_ADDRESS, sizeof(DEFAULT_ADDRESS));
}

void RangeSettings::reset(){
  for(int i=0; i<4; ++i){
    min[i] = -1;
    max[i] = 1;
  }
}
