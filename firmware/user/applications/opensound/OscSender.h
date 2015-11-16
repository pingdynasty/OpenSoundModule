#ifndef __OscSender_h__
#define __OscSender_h__

#include "OscMessage.hpp"

class OscSender {
public:
  enum OscMessageId {
    STATUS = 0,
    CV_A,
    CV_B,
    TRIGGER_A,
    TRIGGER_B,
    MESSAGE_COUNT
  };
  OscMessage messages[MESSAGE_COUNT];
  OscSender();
  void init();

  void setAddress(OscMessageId mid, char* address){
    if(mid < MESSAGE_COUNT)
      messages[mid].setAddress(address);
  }

  char* getAddress(OscMessageId mid){
    if(mid < MESSAGE_COUNT)
      return messages[mid].getAddress();
    return NULL;
  }

  void send(OscMessageId mid, int value);
  void send(OscMessageId mid, float value);
  void send(OscMessageId mid, const char* value);
};

#endif /* __OscSender_h__ */
