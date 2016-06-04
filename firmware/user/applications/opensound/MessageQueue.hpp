
#define MESSAGE_QUEUE_MAX_MESSAGES 16

#include "OscMessage.hpp"

class CircularBuffer;

class MessageQueue {
  OscMessage messages[MESSAGE_QUEUE_MAX_MESSAGES]; // an OscMessage is 16 bytes
  volatile uint8_t head;
  volatile uint8_t tail;
private:
public:
  OscMessage* popNextMessage(){
    if(tail == head)
      return NULL;
    OscMessage* msg = messages[tail];
    if(++tail >= MESSAGE_QUEUE_MAX_MESSAGES)
      tail = 0;
    return msg;
  }
// note:
// if circular buffer is used for msg data, a msg must not cross buffer boundary
  OscMessage* pushNextMessage(uint8_t* msg, int size){
    if(++head >= MESSAGE_QUEUE_MAX_MESSAGES)
      head = 0;
    if(head == tail){
      // risk of clobbering a message while it is processed: reverse instead
      // todo: signal a buffer overflow
      if(head-- >= MESSAGE_QUEUE_MAX_MESSAGES)
	head = MESSAGE_QUEUE_MAX_MESSAGES-1;
      return;
    }
    messages[head].setBuffer(msg, size);
  }
  
};
