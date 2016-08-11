#ifndef OPENSOUND_H
#define OPENSOUND_H

#include <stdint.h>
#include <stddef.h>

#define SERIAL_DEBUG
#define SERIAL_CONSOLE
#define SERVICE_OSC
#define SERVICE_MDNS
#define SERVICE_WEBSOCKETS

#define WEBSOCKET_BUFFER_SIZE       64
#define WEBSOCKET_SERVER_PORT       8008

#define OSC_ADDRESS_MAX_LEN         19
#define OSC_MESSAGE_SIZE            (OSC_ADDRESS_MAX_LEN+1+4+4)
#define OSC_MESSAGE_COUNT           5
#define ANALOG_THRESHOLD            31

#define FIRMWARE_VERSION            "v0.9"
#define OSM_AP_HOSTNAME             "OpenSound"
#define OSM_AP_AUTH                 "3"

#define ASSERT(cond, msg) if(!(cond)){assert_failed(msg, __PRETTY_FUNCTION__, __LINE__);}

#define NETWORK_LOCAL_WIFI   0
#define NETWORK_ACCESS_POINT 1

// pin mappings
#define ANALOG_PIN_A         A0
#define ANALOG_PIN_B         A1
#define DIGITAL_OUTPUT_PIN_A D0
#define DIGITAL_OUTPUT_PIN_B D1
#define DIGITAL_INPUT_PIN_A  D2
#define DIGITAL_INPUT_PIN_B  D3
#define BUTTON_PIN           D6
#define GREEN_LED_PIN        D4
#define YELLOW_LED_PIN       D5

#ifdef  __cplusplus

#include "application.h"

template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }

enum LedPin {
  LED_NONE,
  LED_GREEN,
  LED_YELLOW
};

void setLed(LedPin led);
void printInfo(Print& out);

class Debug : public Print {
  size_t write(uint8_t data){
#ifdef SERIAL_DEBUG
    return Serial.write(data);
#else
    return 1;
#endif
  }
  size_t write(const uint8_t* data, size_t size){
#ifdef SERIAL_DEBUG
    return Serial.write(data, size);
#else
    return size;
#endif
  }
};
extern Debug debug;

#endif

#ifdef  __cplusplus
extern "C" {
#endif
  void startServers();
  void stopServers();
  void reload();
  void setRemoteIpAddress(const char* ip);
  void toggleLed();
  uint16_t getCVA();
  uint16_t getCVB();
  void setCVA(uint16_t cv);
  void setCVB(uint16_t cv);
  void setTriggerA(int value);
  void setTriggerB(int value);
  void toggleTriggerA();
  void toggleTriggerB();
  void factoryReset();
  const char* getDeviceName();
  void setDeviceName(const char* name);
  void broadcastStatus();
  void debugMessage(const char* msg);
  void assert_failed(const char* msg, const char* location, int line);

  void process_opensound(uint8_t* data, size_t size);

#ifdef  __cplusplus
}
#endif

#endif  /* OPENSOUND_H */
