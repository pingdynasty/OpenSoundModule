#ifndef __OscSender_h__
#define __OscSender_h__

#include "opensound.h"
#include "OscMessage.hpp"

class OscSender {
public:
  enum OscMessageId {
    STATUS = 0,
    CV_A,
    CV_B,
    TRIGGER_A,
    TRIGGER_B,
  };
  OscMessage messages[OSC_MESSAGE_COUNT];
  uint8_t buffer[OSC_MESSAGE_SIZE*OSC_MESSAGE_COUNT];
  OscSender();
  void init();

  void setAddress(OscMessageId mid, char* address){
    if(mid < OSC_MESSAGE_COUNT)
      messages[mid].setAddress(address);
  }

  char* getAddress(OscMessageId mid){
    if(mid < OSC_MESSAGE_COUNT)
      return messages[mid].getAddress();
    return NULL;
  }

  void send(OscMessageId mid, int value);
  void send(OscMessageId mid, float value);
  void send(OscMessageId mid, const char* value);
};

#endif /* __OscSender_h__ */
