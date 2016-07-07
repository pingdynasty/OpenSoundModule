#ifndef _DigitalBusHandler_h_
#define _DigitalBusHandler_h_

#include "MidiReader.h"
#include "bus.h"

class DigitalBusHandler : public MidiReader {
  enum DigitalBusStatus {
    IDLE = BUS_STATUS_IDLE,
    DISCOVERING = BUS_STATUS_DISCO,
    ENUMERATING = BUS_STATUS_ENUM,
    IDENTIFYING = BUS_STATUS_IDENT,
    CONNECTED = BUS_STATUS_CONNECTED,
    ERROR = BUS_STATUS_ERROR
  };
protected:
  uint8_t uid; // this device id
  uint8_t nuid; // downstream device id
  uint32_t token;
  uint8_t peers;
  uint16_t parameterOffset;
  uint8_t* UUID;
  DigitalBusStatus status = IDLE;  
  static const uint8_t VERSION = 0x01; // protocol version
  static const uint8_t PRODUCT = 0x01;  // product id
  static const uint8_t PARAMETERS = 5; // number of parameters defined by this product
  static const uint8_t NO_UID = 0xff;
  static const uint32_t NO_TOKEN = 0xffffffff;
public:
  DigitalBusHandler();
  bool connected();
  DigitalBusStatus getStatus(){
    return status;
  }
  uint32_t generateToken();
  uint8_t getPeers(){ return peers; }
  uint8_t getUid(){ return uid; }
  uint8_t getNuid(){ return nuid; }
  void startDiscover();
  void sendDiscover(uint8_t seq, uint32_t token);
  void handleDiscover(uint8_t seq, uint32_t other);
  void startEnum();
  void sendEnum(uint8_t id, uint8_t version, uint8_t product, uint8_t params);
  void handleEnum(uint8_t id, uint8_t version, uint8_t product, uint8_t params);
  void startIdent();
  void sendIdent(uint8_t id, uint8_t version, uint8_t device, uint8_t* uuid);
  void handleIdent(uint8_t id, uint8_t d1, uint8_t d2, uint8_t d3);
  void sendParameterChange(uint8_t pid, int16_t value);
  void handleParameterChange(uint8_t pid, int16_t value);
  void sendButtonChange(uint8_t pid, int16_t value);
  void handleButtonChange(uint8_t pid, int16_t value);
  void sendCommand(uint8_t cmd, int16_t data);
  void handleCommand(uint8_t cmd, int16_t data);
  void sendMessage(const char* msg);
  void handleMessage(const char* msg);
  void sendData(uint8_t* data, uint16_t len);
  void handleData(uint8_t* data, uint16_t len);
protected:
  // send a 4-byte message
  void sendFrame(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);
  void sendFrame(uint8_t* frame);
};

#endif /* _DigitalBusHandler_h_ */
