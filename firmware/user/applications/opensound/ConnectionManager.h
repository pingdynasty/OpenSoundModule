#ifndef __ConnectionManager_h__
#define __ConnectionManager_h__

//#include "ApplicationSettings.h"
#include <stdint.h>

#define CONNECTION_TIMEOUT 20000
#define FALTERING_TIMEOUT  20000

class IPAddress;
class Print;
class String;

class ConnectionManager {
  enum OpenSoundMode {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    FALTERING,
    DISCONNECTING
  };
  enum ServiceType {
    WIFI         = 0x01,
    DNS_REDIRECT = 0x02,
    SERVERS      = 0x04,
  };

public:
  ConnectionManager();
  bool connected();

  OpenSoundMode getMode(){
    return mode;
  }
  void setMode(OpenSoundMode m);

  bool getStatus(ServiceType type){
    return (status & type) != 0;
  }

  void setStatus(ServiceType type, bool on){
    if(on)
      status |= type;
    else
      status &= ~type;
  }

  void connect(int iface);
  void disconnect();
  void cancel();

  int getCurrentNetwork(){
    return selected_network;
  }

  IPAddress getLocalIPAddress();
  IPAddress getSubnetMask();
  IPAddress getDefaultGateway();
  void printMacAddress(Print& out);
  bool isWiFiConnected();
  bool isIpConnected();
  const char* getSSID();
  int getRSSI();
  bool setCredentials(const char* ssid, const char* password, const char* auth);
  void clearCredentials();
  bool hasCredentials();
  bool setAccessPointCredentials(const char* ssid, const char* passwd, const char* auth);
  void setAccessPointPrefix(const char* prefix);
  String getAccessPointPrefix();
  const char* getAccessPointSSID();
  void updateLed();
  bool start(ServiceType type);
  bool stop(ServiceType type);
  const char* getHostname();
  void setHostname(const char* name);
  bool generateAccessPointCredentials();
private:  
  unsigned long lastEvent;
  OpenSoundMode mode;
  int selected_network, next_network;
  int failures;
  uint8_t status;
};

extern ConnectionManager connection;

#endif /* __ConnectionManager_h__ */
