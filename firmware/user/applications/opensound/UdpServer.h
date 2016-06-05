#include "opensound.h"

#ifndef __UDP_SERVER_H
#define __UDP_SERVER_H

#include "spark_wiring.h"
#include "spark_wiring_printable.h"
#include "spark_wiring_stream.h"
#include "socket_hal.h"

#define UDP_TX_BUF_MAX_SIZE	128
#define UDP_RX_BUF_MAX_SIZE	256

class UdpServer : public Stream, public Printable {
protected:
  sock_handle_t _sock;
  uint16_t _port;
  IPAddress _remoteIP;
  uint16_t _remotePort;
  sockaddr_t _remoteSockAddr;
  socklen_t _remoteSockAddrLen;
  uint8_t _rxbuffer[UDP_TX_BUF_MAX_SIZE];
  uint8_t _txbuffer[UDP_RX_BUF_MAX_SIZE];
  uint16_t _rxoffset;
  uint16_t _txoffset;
  uint16_t _total;
  network_interface_t _nif;
public:
  UdpServer();
  ~UdpServer(){}
  uint8_t begin(uint16_t, network_interface_t nif=0);
  void stop();
  int beginPacket(IPAddress ip, uint16_t port);
  int beginPacket(const char *host, uint16_t port);
  int endPacket();
  size_t write(uint8_t);
  size_t write(const uint8_t *buffer, size_t size);
  int parsePacket();
  int available();
  int read();
  int read(unsigned char* buffer, size_t len);
  int read(char* buffer, size_t len) { return read((unsigned char*)buffer, len); };
  int peek();
  void flush();
  IPAddress remoteIP() { return _remoteIP; };
  uint16_t remotePort() { return _remotePort; };

  /**
   * Prints the current read parsed packet to the given output.
   * @param p
   * @return
   */
  virtual size_t printTo(Print& p) const;

  using Print::write;
};
#endif /* __UDP_SERVER_H */
