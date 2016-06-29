#include "OscServer.h"
#include "opensound.h"
#include "ApplicationSettings.h"

OscServer::OscServer() : commandCount(0) {
}

void OscServer::init(){
  commandCount = 0;
  prefix = settings.outputPrefix;
  prefixLen = strlen(prefix);
  setRemoteIP(settings.remoteIPAddress);
  setRemotePort(settings.remotePort);
  setBroadcastMode(settings.broadcast);
  autoRemoteIPAddress = settings.autoremote;
}

void OscServer::setBroadcastMode(bool broadcast){
  if(broadcast){
    remoteIPAddress = WiFi.localIP();
    remoteIPAddress[3] = 255;
#ifdef SERIAL_DEBUG
    Serial.print("Remote IP (broadcast): ");
    Serial.println(remoteIPAddress);
#endif
  }else{
    remoteIPAddress = settings.remoteIPAddress;
#ifdef SERIAL_DEBUG
    Serial.print("Remote IP: ");
    Serial.println(remoteIPAddress);
#endif
  }
}

void OscServer::udp_recv_packet(uint8_t* buffer, int size){
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

void OscServer::processMessage(uint8_t* buffer, int size){
#ifdef UDP_SERIAL_DEBUG
  Serial.print("osc message ");
  Serial.println(size);
#endif
  OscMessage msg(buffer, size);
  msg.parse();
  const char* address = msg.getAddress();
  if(strncmp(prefix, address, prefixLen) == 0){
    address += prefixLen;
    uint8_t args = msg.getSize();
    for(int i=0; i<commandCount; ++i){
      if(commands[i].matches(address, args)){
	commands[i].cmd(*this, msg);
	return;
      }
    }
    for(int i=0; i<16; ++i){
      if(strcmp(PARAMETER_NAMES[i], address) == 0)
	bus_tx_parameter(i, (int16_t)(msg.getAsFloat(0)*4096));
    }
  }
}

void OscServer::sendMessage(OscMessage& msg){
  beginPacket();
  write(msg.getBuffer(), msg.calculateMessageLength());
  endPacket();
}
