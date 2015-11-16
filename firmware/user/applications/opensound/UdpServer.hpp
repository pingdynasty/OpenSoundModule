#include "opensound.h"

class UdpServer : public UDP {
private :
  uint8_t txbuffer[UDP_TX_BUFFER_SIZE];
  uint8_t rxbuffer[UDP_RX_BUFFER_SIZE];
  int txoffset;
public :
  int remotePort;
  IPAddress remoteIPAddress;
  bool autoRemoteIPAddress = true;
  UdpServer() : txoffset(0) {}
  int beginPacket(){
    return beginPacket(remoteIPAddress, remotePort);
  }
  int beginPacket(IPAddress ip, uint16_t port){
    txoffset = 0;
    return UDP::beginPacket(ip, port);
  };
  int endPacket(){
    return UDP::write(txbuffer, txoffset);
  };
  size_t write(uint8_t data) {
    write(&data, 1);
    return 1;
  }
  size_t write(const uint8_t *buffer, size_t size) {
    if(size+txoffset > UDP_TX_BUFFER_SIZE){
      debugMessage("udp tx buffer overflow");
      return 0;
    }
    memcpy(&txbuffer[txoffset], buffer, size);
    txoffset += size;
    return size;
  }
  // size_t write(OscMessage& msg){
  //   msg.write(*this);
  // }
  void loop(){
    int len = parsePacket();
    if(len > 0) {
#ifdef SERIAL_DEBUG
	Serial.print("udp recv ");
	Serial.print(len);
	Serial.print('/');
	Serial.println(UDP_RX_BUFFER_SIZE);
#endif
      if(autoRemoteIPAddress){
	remoteIPAddress = remoteIP();
#ifdef SERIAL_DEBUG
	Serial.print("Remote IP (auto): ");
	Serial.println(remoteIPAddress);
#endif
      }
      len = min(len, UDP_RX_BUFFER_SIZE);
      len = read(rxbuffer, len);
      if(len > 0){
	udp_recv_packet(rxbuffer, len);
      }else{
	debugMessage("udp read error");
      }
    }
  }
  virtual void udp_recv_packet(uint8_t* buffer, int size){}
};
