#include "UdpServer.h"

#include "spark_wiring_udp.h"
#include "socket_hal.h"
#include "inet_hal.h"
#include "spark_macros.h"
#include "spark_wiring_network.h"

using namespace spark;

static bool inline isOpen(sock_handle_t sd)
{
   return sd != socket_handle_invalid();
}

UdpServer::UdpServer() : _sock(socket_handle_invalid()), _txoffset(0)
{

}

uint8_t UdpServer::begin(uint16_t port, network_interface_t nif)
{
    bool bound = 0;
	if(Network.from(nif).ready())
	{
	   _sock = socket_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, port, nif);
            DEBUG("socket=%d",_sock);
            if (socket_handle_valid(_sock))
            {
                flush();
                _port = port;
                _nif = nif;
		bound = 1;
            }
	}
    return bound;
}

int UdpServer::available()
{
    return _total - _rxoffset;
}

void UdpServer::stop()
{
    DEBUG("_sock %d closesocket", _sock);
    if (isOpen(_sock))
    {
        socket_close(_sock);
    }
    _sock = socket_handle_invalid();
}

int UdpServer::beginPacket(const char *host, uint16_t port){
    _txoffset = 0;
    if(Network.from(_nif).ready())
    {
        HAL_IPAddress ip_addr;

        if(inet_gethostbyname((char*)host, strlen(host), &ip_addr, _nif, NULL) == 0)
        {
            IPAddress remote_addr(ip_addr);
            return beginPacket(remote_addr, port);
        }
    }
	return 0;
}

int UdpServer::beginPacket(IPAddress ip, uint16_t port){
    _txoffset = 0;
	_remoteIP = ip;
	_remotePort = port;

	_remoteSockAddr.sa_family = AF_INET;

	_remoteSockAddr.sa_data[0] = (_remotePort & 0xFF00) >> 8;
	_remoteSockAddr.sa_data[1] = (_remotePort & 0x00FF);

	_remoteSockAddr.sa_data[2] = _remoteIP[0];
	_remoteSockAddr.sa_data[3] = _remoteIP[1];
	_remoteSockAddr.sa_data[4] = _remoteIP[2];
	_remoteSockAddr.sa_data[5] = _remoteIP[3];

	_remoteSockAddrLen = sizeof(_remoteSockAddr);

	return 1;
}

int UdpServer::endPacket(){
  int rv =  socket_sendto(_sock, _txbuffer, _txoffset, 0, &_remoteSockAddr, _remoteSockAddrLen);
  DEBUG("sendto(buffer=%lx, size=%d)=%d",_txbuffer, _txoffset , rv);
  _txoffset = 0;
  return rv;
	//	return 1;
}

size_t UdpServer::write(uint8_t byte){
  return write(&byte, 1);
}

size_t UdpServer::write(const uint8_t *buffer, size_t size){
  if(size+_txoffset > UDP_TX_BUF_MAX_SIZE){
    DEBUG("udp tx buffer overflow");
    return 0;
  }
  memcpy(&_txbuffer[_txoffset], buffer, size);
  _txoffset += size;
  return size;
#if 0
  int rv =  socket_sendto(_sock, buffer, size, 0, &_remoteSockAddr, _remoteSockAddrLen);
  DEBUG("sendto(buffer=%lx, size=%d)=%d",buffer, size , rv);
  return rv;
#endif
}

int UdpServer::parsePacket(){
  // No data buffered
    if(available() == 0 && Network.from(_nif).ready() && isOpen(_sock))
    {
        int ret = socket_receivefrom(_sock, _rxbuffer, arraySize(_rxbuffer), 0, &_remoteSockAddr, &_remoteSockAddrLen);
        if (ret > 0)
        {
            _remotePort = _remoteSockAddr.sa_data[0] << 8 | _remoteSockAddr.sa_data[1];

            _remoteIP[0] = _remoteSockAddr.sa_data[2];
            _remoteIP[1] = _remoteSockAddr.sa_data[3];
            _remoteIP[2] = _remoteSockAddr.sa_data[4];
            _remoteIP[3] = _remoteSockAddr.sa_data[5];

            _rxoffset = 0;
            _total = ret;
        }
    }
    return available();
}

int UdpServer::read()
{
  return available() ? _rxbuffer[_rxoffset++] : -1;
}

int UdpServer::read(unsigned char* buffer, size_t len)
{
        int read = -1;
        if (available())
	{
          read = (len > (size_t) available()) ? available() : len;
          memcpy(buffer, &_rxbuffer[_rxoffset], read);
          _rxoffset += read;
	}
	return read;
}

int UdpServer::peek()
{
     return available() ? _rxbuffer[_rxoffset] : -1;
}

void UdpServer::flush()
{
  _rxoffset = 0;
  _total = 0;

}

size_t UdpServer::printTo(Print& p) const
{
    // can't use available() since this is a `const` method, and available is part of the Stream interface, and is non-const.
    int size = _total - _rxoffset;
    return p.write(_rxbuffer+_rxoffset, size);
}
