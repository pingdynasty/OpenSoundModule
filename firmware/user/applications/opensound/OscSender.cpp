#include "application.h"
#include "OscSender.h"
#include "OscServer.h"
#include "ApplicationSettings.h"

extern OscServer oscserver;

OscSender::OscSender(){
}

void setPrefix(OscMessage& msg, const char* prefix, const char* addr, const char* types){
  char buf[24];
  char* p = buf;
  p = stpcpy(p, prefix);
  p = stpcpy(p, addr);
  msg.setPrefix(buf, types);
}

void OscSender::init(){
  messages[0].setBuffer(buffer, 64);
  for(int i=1; i<OSC_MESSAGE_COUNT; ++i)
    messages[i].setBuffer(buffer+64+(i-1)*OSC_MESSAGE_SIZE, OSC_MESSAGE_SIZE);
  setPrefix(messages[STATUS], settings.outputPrefix, settings.outputAddress[0], ",s");
  setPrefix(messages[CV_A], settings.outputPrefix, settings.outputAddress[1], ",f");
  setPrefix(messages[CV_B], settings.outputPrefix, settings.outputAddress[2], ",f");
  setPrefix(messages[TRIGGER_A], settings.outputPrefix, settings.outputAddress[3], ",i");
  setPrefix(messages[TRIGGER_B], settings.outputPrefix, settings.outputAddress[4], ",i");
  // messages[STATUS].setPrefix(settings.outputAddress[0], ",s");
  // messages[CV_A].setPrefix(settings.outputAddress[1], ",f");
  // messages[CV_B].setPrefix(settings.outputAddress[2], ",f");
  // messages[TRIGGER_A].setPrefix(settings.outputAddress[3], ",i");
  // messages[TRIGGER_B].setPrefix(settings.outputAddress[4], ",i");
}


void OscSender::sendInt(OscMessageId mid, int value){
  if(mid < OSC_MESSAGE_COUNT){ 
    messages[mid].setInt(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    // messages[mid].write(oscserver);
    oscserver.endPacket();
  }
}

void OscSender::sendFloat(OscMessageId mid, float value){
  if(mid < OSC_MESSAGE_COUNT){
    messages[mid].setFloat(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    oscserver.endPacket();
  }else if(mid >= PARAMETER_AA && mid <= PARAMETER_BH){
    uint8_t buf[OSC_MESSAGE_SIZE];
    OscMessage msg(buf, OSC_MESSAGE_SIZE);
    setPrefix(msg, settings.outputPrefix, PARAMETER_NAMES[mid-PARAMETER_AA], ",f");
    msg.setFloat(0, value);
    debug << "sending [" << msg.getAddress() << "][" << msg.getAsFloat(0) << "]\r\n";
    oscserver.beginPacket();
    oscserver.write(msg.getBuffer(), msg.calculateMessageLength());
    oscserver.endPacket();
  }
}

void OscSender::sendString(OscMessageId mid, const char* value){
  if(mid < OSC_MESSAGE_COUNT){
    messages[mid].setString(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    oscserver.endPacket();
  }
}
