#include "OscServer.h"
#include "opensound.h"
#include "ApplicationSettings.h"

void OscServer::init(){
  commandCount = 0;
  setRemoteIP(networkSettings.remoteIPAddress);
  setRemotePort(networkSettings.remotePort);
  setBroadcastMode(networkSettings.broadcast);
  autoRemoteIPAddress = networkSettings.autoremote;
}

void OscServer::setBroadcastMode(bool broadcast){
  if(broadcast){
    remoteIPAddress = WiFi.localIP();
    remoteIPAddress[3] = 255;
#ifdef SERIAL_DEBUG
    Serial.print("Remote IP (broadcast): ");
    Serial.println(remoteIPAddress);
#endif
  }else{
    remoteIPAddress = networkSettings.remoteIPAddress;
#ifdef SERIAL_DEBUG
    Serial.print("Remote IP: ");
    Serial.println(remoteIPAddress);
#endif
  }
}
