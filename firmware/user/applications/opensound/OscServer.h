#ifndef __OscServer_h__
#define __OscServer_h__

#include "OscMessage.hpp"
#include "ApplicationSettings.h"
#include "UdpServer.h"

#define OSC_MAX_COMMANDS 10
// #define UDP_SERIAL_DEBUG

class OscServer : public UdpServer {
  typedef void OscCommand(OscServer &server, OscMessage& msg);
  struct OscCommandMap {
    const char* address;
    uint8_t minArgs;
    OscCommand* cmd;
    /* bool matches(OscMessage& msg){ */
    /*   return strncmp(msg.getAddress(), address, OSC_ADDRESS_MAX_LEN) == 0 && msg.getSize() >= minArgs; */
    /* } */
    bool matches(const char* addr, uint8_t len){
      return len >= minArgs && strncmp(addr, address, OSC_ADDRESS_MAX_LEN) == 0;
    }
  };
  uint8_t commandCount;
  OscCommandMap commands[OSC_MAX_COMMANDS];
  char* prefix;
  uint8_t prefixLen;
public:
  OscServer();
  void init();

  void loop(){
    int len = parsePacket();
    if(len > 0) {
#ifdef UDP_SERIAL_DEBUG
      Serial.print("udp recv ");
      Serial.print(len);
      Serial.print('/');
      Serial.println(UDP_RX_BUF_MAX_SIZE);
#endif
      if(autoRemoteIPAddress){
	remoteIPAddress = remoteIP();
#ifdef UDP_SERIAL_DEBUG
	Serial.print("Remote IP (auto): ");
	Serial.println(remoteIPAddress);
#endif
      }
      udp_recv_packet(_rxbuffer, len);
      _rxoffset += len;
      //      len = min(len, UDP_RX_BUFFER_SIZE);
      //      len = read(_rxbuffer, len);
      //      if(len > 0){
      //      udp_recv_packet(rxbuffer, len);
	//	}else{
	//	debugMessage("udp read error");
    }
  }
  bool autoRemoteIPAddress;
  IPAddress remoteIPAddress;
  uint16_t remotePortNumber;
  int beginPacket(){
    return UdpServer::beginPacket(remoteIPAddress, remotePortNumber);
  }
  void setRemoteIP(IPAddress ip){
    remoteIPAddress = ip;
  }
  void setRemotePort(uint16_t port){
    remotePortNumber = port;
  }
  void reset(){
    commandCount = 0;
  }
  void addCommand(const char* address, OscCommand* cmd, int minArgs = 0){
    if(commandCount < OSC_MAX_COMMANDS){
      commands[commandCount].address = address;
      commands[commandCount].minArgs = minArgs;
      commands[commandCount].cmd = cmd;    
      commandCount++;
    }
  }

  const char* getPrefix(){
    return prefix;
  }

  const char* getAddress(int cmd){
    if(cmd < commandCount)
      return commands[cmd].address;
    return NULL;
  }

  void setAddress(int cmd, const char* address){
    if(cmd < commandCount){
      //      strncpy(commands[cmd].address, address, OSC_ADDRESS_MAX_LEN);
      //      commands[cmd].address[OSC_ADDRESS_MAX_LEN-1] = '\0';
      commands[cmd].address = address;
    }
  }

  void udp_recv_packet(uint8_t* buffer, int size);
  void processMessage(uint8_t* buffer, int size);
  void sendMessage(OscMessage& msg);

  bool isAutoMode(){
    return autoRemoteIPAddress;
  }

  bool isBroadcastMode(){
    return remoteIPAddress[3] == 255;
  }

  void setBroadcastMode(bool broadcast);
};

#endif /* __OscServer_h__ */
