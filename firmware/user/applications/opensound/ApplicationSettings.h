#ifndef __ApplicationSettings_H__
#define __ApplicationSettings_H__

#include <inttypes.h>
/* #include "application.h" */
#include "opensound.h"

#define APPLICATION_SETTINGS_ADDRESS 0
/* The photon has 2048 bytes of emulated EEPROM. */

#define CV_A_IN  0
#define CV_B_IN  1
#define CV_A_OUT 2
#define CV_B_OUT 3

class ApplicationSettings {
private:
  uint8_t checksum; // must be first in struct
  /* the idea is that if anything is added or removed from this class then
     the checksum changes, preventing a corrupted read-back. */
public:
  void reset();
  void init(){
    checksum = (uint8_t)sizeof(*this) ^ 0xff;
    if(settingsInFlash())
      loadFromFlash();
    else
      reset();
  }
  bool settingsInFlash(){
    return EEPROM.read(APPLICATION_SETTINGS_ADDRESS) == checksum;
  }
  bool equals(const ApplicationSettings& other){
    return memcmp(this, &other, sizeof(*this)) == 0;
  }
  bool hasChanged(){
    if(settingsInFlash()){
      ApplicationSettings other;
      other.init();
      if(equals(other))
	return false;
    }
    return true;
  }
  void loadFromFlash(){
    EEPROM.get(APPLICATION_SETTINGS_ADDRESS, *this);
  }
  void saveToFlash(){
    EEPROM.put(APPLICATION_SETTINGS_ADDRESS, *this);
  }
  void clearFlash(){
    EEPROM.write(APPLICATION_SETTINGS_ADDRESS, 0x00);
  }
public:
  int localPort;
  int remotePort;
  bool broadcast;
  bool autoremote;
  IPAddress remoteIPAddress;
public:
  // todo: let char* live in OscSender.messages
  // to read, write, compare flash load to/from messages
  /* char inputAddress[5][OSC_ADDRESS_MAX_LEN]; */
  /* char outputAddress[5][OSC_ADDRESS_MAX_LEN]; */
  char inputPrefix[16];
  char outputPrefix[16];
  char inputAddress[OSC_MESSAGE_COUNT][OSC_ADDRESS_MAX_LEN+1];
  char outputAddress[OSC_MESSAGE_COUNT][OSC_ADDRESS_MAX_LEN+1];
  const char* getInputAddress(int i){
    return inputAddress[i];
  }
  const char* getOutputAddress(int i){
    return outputAddress[i];
  }
  void setInputPrefix(const char* address){
    strncpy(inputPrefix, address, sizeof(inputPrefix)-1);
  }
  void setOutputPrefix(const char* address){
    strncpy(outputPrefix, address, sizeof(outputPrefix)-1);
  }
  void setInputAddress(int i, const char* address){
    strncpy(inputAddress[i], address, OSC_ADDRESS_MAX_LEN);
  }
  void setOutputAddress(int i, const char* address){
    strncpy(outputAddress[i], address, OSC_ADDRESS_MAX_LEN);
  }
  float min[4];
  float max[4];
};

extern ApplicationSettings settings;

#endif // __ApplicationSettings_H__
