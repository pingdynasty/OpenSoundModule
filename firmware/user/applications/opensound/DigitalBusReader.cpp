#include "DigitalBusReader.h"
#include "message.h"
#include "bus.h"
#include <string.h>

void DigitalBusReader::appendFrame(uint8_t* frame){

}

// read a 4-byte data frame
void DigitalBusReader::readBusFrame(uint8_t* frame){
  debug << "rx bus [" << frame[0] << "]\r\n";
  // OWL Digital Bus Protocol
  uint8_t id = frame[0]&0x0f;
  switch(frame[0]&0xf0){
  case 0:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    readMidiFrame(frame);
    break;
  case OWL_COMMAND_DISCOVER:
    handleDiscover(id, (frame[1] << 16) | (frame[2]<<8) | frame[3]);
    break;
  case OWL_COMMAND_ENUM:
    if(peers == 0)
      return bus_rx_error("Out of sequence message");
    handleEnum(id, frame[1], frame[2], frame[3]);
    break;
  case OWL_COMMAND_IDENT:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    if(id != uid){
      handleIdent(id, frame[1], frame[2], frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_PARAMETER:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    if(id != uid){
      // it's not from us: process
      handleParameterChange(frame[1], (frame[2]<<8) | frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_BUTTON:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    if(id != uid){
      handleButtonChange(frame[1], (frame[2]<<8) | frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_COMMAND:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    if(id != uid){
      handleCommand(frame[1], (frame[2]<<8) | frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_MESSAGE:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    if(id != uid){
      // if(appendFrame(frame));
      if(txuid == NO_UID)
	txuid = id;
      if(txuid == id){
	// ignore if we are not exclusively listening to long messages from this uid
	if(pos+3 < sizeof(buffer)){
	  strncpy((char*)buffer+pos, (char*)frame+1, 3);
	  pos += 3;
	}else{
	  // buffer overflow
	  pos = 0;
	}
	if(frame[3] == '\0'){
	  pos = 0;
	  txuid = NO_UID;
	  handleMessage((const char*)buffer);
	}
      }
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_DATA:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    // todo: read size, use buffer to store
    // OSC, firmware or file data
    // 0x30, type, sizeH, sizeL
    // uint16_t size = (frame[2]<<8) | frame[3]);
    break;
  case OWL_COMMAND_SYNC:
    if(nuid == NO_UID)
      return bus_rx_error("Out of sequence message");
    // 0xc0 until 0xff at end of frame
    // use ASCII SYN instead?
    break;
  default:
    debug << "rx error [" << frame[0] << "]\r\n";
    bus_rx_error("Invalid message");
    break;
  }
}
