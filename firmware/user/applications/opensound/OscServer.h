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
    bool matches(OscMessage& msg){
      return strncmp(msg.getAddress(), address, OSC_ADDRESS_MAX_LEN) == 0 && msg.getSize() >= minArgs;
    }
  };
  int commandCount;
  OscCommandMap commands[OSC_MAX_COMMANDS];
public:
  OscServer() : commandCount(0) {}

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

  void udp_recv_packet(uint8_t* buffer, int size){
    if(size >= 28 && strncmp((const char*)buffer, "#bundle", 7) == 0){
      int len = OscMessage::getOscInt32(buffer+16);
      buffer += 16; // discard #bundle and timestamp
      size -= 16;
#ifdef UDP_SERIAL_DEBUG
      Serial.print("#bundle ");
      Serial.print(len);
      Serial.print('/');
      Serial.println(size);
#endif
      while(len >= 8 && size >= len+4){
	processMessage(buffer+4, len);
	buffer += len+4;
	size -= len+4;
	len = size >= 12 ? OscMessage::getOscInt32(buffer) : 0;
#ifdef UDP_SERIAL_DEBUG
      Serial.print("next ");
      Serial.print(len);
      Serial.print('/');
      Serial.println(size);
#endif
      }
    }else if(size >= 8 && buffer[0] == '/'){
      processMessage(buffer, size);
    } // else ignore
  }

  void processMessage(uint8_t* buffer, int size){
#ifdef UDP_SERIAL_DEBUG
    Serial.print("osc message ");
    Serial.println(size);
#endif
    OscMessage msg(buffer, size);
    msg.parse();
    for(int i=0; i<commandCount; ++i)
      if(commands[i].matches(msg)){
	commands[i].cmd(*this, msg);
	break;
      }
  }

  void sendMessage(OscMessage& msg){
    beginPacket();
    write(msg.getBuffer(), msg.calculateMessageLength());
    endPacket();
  }

  bool isAutoMode(){
    return autoRemoteIPAddress;
  }

  bool isBroadcastMode(){
    return remoteIPAddress[3] == 255;
  }

  void setBroadcastMode(bool broadcast);
};

#endif /* __OscServer_h__ */
