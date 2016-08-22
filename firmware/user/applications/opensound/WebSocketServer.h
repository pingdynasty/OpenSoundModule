#ifndef WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include "opensound.h"
// #include "websocket.h"
  
class WebSocketServer {
private:
  enum WebSocketService {
    NO_SERVICE,
    ECHO_SERVICE,
    STATUS_SERVICE,
    MIDI_SERVICE,
    OSC_SERVICE
  };
  WebSocketService service;
public:
WebSocketServer(const unsigned port);

 void begin();
 void stop();

 void loop();

  void reset(){
    service = NO_SERVICE;
  }

  int processHandshake(const char* uri, size_t len);

  void sendOscData(uint8_t* data, size_t dataSize){
    if(service == OSC_SERVICE){
      sendBinaryFrame(data, dataSize);
    }
  }

  void processOscData(uint8_t* data, size_t dataSize){
    process_osc(data, dataSize);
  }

  void processTextFrame(char* data, size_t dataSize){
    switch(service){
    case ECHO_SERVICE:
      processEcho(data, dataSize);
      break;
    case STATUS_SERVICE:
      processStatus(data, dataSize);
      break;
    default:
      break;
    }
  }    

  void processBinaryFrame(uint8_t* data, size_t dataSize){
    switch(service){
    case OSC_SERVICE:
      processOscData(data, dataSize);
      break;
    default:
      break;
    }
  }    

  void processStatus(char* data, size_t dataSize){
    const char msg[] = "I'm fine, thanks, how are you?";
    sendTextFrame(msg, sizeof(msg)-1);
  }

  void processEcho(char* data, size_t dataSize){
#ifdef SERIAL_DEBUG
    data[dataSize] = '\0';
    debug << "ws echo [" << data << "]\r\n";
#endif
    sendTextFrame(data, dataSize);
  }

  void sendBinaryFrame(uint8_t* data, size_t dataSize);

  void sendTextFrame(const char* data, size_t dataSize);
};
#endif /* WEBSOCKETSERVER_H_ */
