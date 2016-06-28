#include "DigitalBusReader.h"
#include "opensound.h"

// read a 4-byte data frame
void DigitalBusReader::readBusFrame(uint8_t* frame){
  debug << "rx bus [" << frame[0] << "]\r\n";
  // OWL Digital Bus Protocol
  uint8_t id = frame[0]&0x0f;
  switch(frame[0]&0xf0){
  case 0:
    readMidiFrame(frame);
    break;
  case OWL_COMMAND_DISCOVER:
    handleDiscover(id, (frame[1] << 16) | (frame[2]<<8) | frame[3]);
    break;
  case OWL_COMMAND_ENUM:
    handleEnum(id, frame[1], frame[2], frame[3]);
    break;
  case OWL_COMMAND_IDENT:
    if(id != uid){
      handleIdent(id, frame[1], frame[2], frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_PARAMETER:
    if(id != uid){
      // it's not from us: process
      handleParameterChange(frame[1], (frame[2]<<8) | frame[3]);
      if(id != nuid) // propagate
	sendFrame(frame);
    }
    break;
  case OWL_COMMAND_DATA:
    // OSC, firmware or file data
    // 0x30, type, sizeH, sizeL
    // uint16_t size = (frame[2]<<8) | frame[3]);
    break;
  case OWL_COMMAND_SYNC:
    // 0xc0 until 0xff at end of frame
    // use ASCII SYN instead?
    break;
  default:
    debug << "rx error [" << frame[0] << "]\r\n";
    rxError("Invalid message");
    break;
  }
}
