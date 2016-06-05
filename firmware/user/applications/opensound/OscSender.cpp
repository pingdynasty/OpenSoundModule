#include "application.h"
#include "OscSender.h"
#include "OscServer.h"
#include "ApplicationSettings.h"

extern OscServer oscserver;

OscSender::OscSender(){
}

void OscSender::init(){
  for(int i=0; i<OSC_MESSAGE_COUNT; ++i)
    messages[i].setBuffer(buffer+i*OSC_MESSAGE_SIZE, OSC_MESSAGE_SIZE);
  messages[STATUS].setPrefix(addressSettings.outputAddress[0], ",s");
  messages[CV_A].setPrefix(addressSettings.outputAddress[1], ",f");
  messages[CV_B].setPrefix(addressSettings.outputAddress[2], ",f");
  messages[TRIGGER_A].setPrefix(addressSettings.outputAddress[3], ",i");
  messages[TRIGGER_B].setPrefix(addressSettings.outputAddress[4], ",i");
}


void OscSender::send(OscMessageId mid, int value){
  if(mid < OSC_MESSAGE_COUNT){ 
    messages[mid].setInt(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    // messages[mid].write(oscserver);
    oscserver.endPacket();
  }
}

void OscSender::send(OscMessageId mid, float value){
  if(mid < OSC_MESSAGE_COUNT){
    messages[mid].setFloat(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    oscserver.endPacket();
  }
}

void OscSender::send(OscMessageId mid, const char* value){
  if(mid < OSC_MESSAGE_COUNT){
    messages[mid].setString(0, value);
    oscserver.beginPacket();
    oscserver.write(messages[mid].getBuffer(), messages[mid].calculateMessageLength());
    oscserver.endPacket();
  }
}
