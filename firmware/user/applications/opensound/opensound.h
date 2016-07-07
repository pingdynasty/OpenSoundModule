#ifndef OPENSOUND_H
#define OPENSOUND_H

#include <stdint.h>
#include <stddef.h>
#include "message.h"

#define SERIAL_CONSOLE
#define SERVICE_MDNS

#define SERVICE_BUS
#define DIGITAL_BUS_BAUD    115200

#define OSC_ADDRESS_MAX_LEN 19
#define OSC_MESSAGE_SIZE    (OSC_ADDRESS_MAX_LEN+1+4+4)
#define OSC_MESSAGE_COUNT   5
#define ANALOG_THRESHOLD    31

#define FIRMWARE_VERSION    "v0.9bus"
#define OSM_AP_HOSTNAME     "OpenSound"
#define OSM_AP_AUTH         "3"

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

#ifdef SERVICE_BUS
#include "bus.h"
#include "serial.h"
#endif

#ifdef  __cplusplus

#include "application.h"

static const char* PARAMETER_NAMES[16] = { 
  "/aa", "/ab", "/ac", "/ad", "/ae", "/af", "/ag", "/ah", 
  "/ba", "/bb", "/bc", "/bd", "/be", "/bf", "/bg", "/bh" 
};

enum LedPin {
  LED_NONE,
  LED_GREEN,
  LED_YELLOW
};

void setLed(LedPin led);
void printInfo(Print& out);

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
#ifdef  __cplusplus
}
#endif

#endif  /* OPENSOUND_H */
