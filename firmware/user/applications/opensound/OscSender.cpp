#include "application.h"
#include "OscSender.h"
#include "OscServer.h"
#include "ApplicationSettings.h"

extern OscServer oscserver;

OscSender::OscSender(){
}

void OscSender::init(){
  messages[0].setBuffer(buffer, 64);
  for(int i=1; i<OSC_MESSAGE_COUNT; ++i)
    messages[i].setBuffer(buffer+64+(i-1)*OSC_MESSAGE_SIZE, OSC_MESSAGE_SIZE);
  messages[STATUS].setPrefix(settings.outputAddress[0], ",s");
  messages[CV_A].setPrefix(settings.outputAddress[1], ",f");
  messages[CV_B].setPrefix(settings.outputAddress[2], ",f");
  messages[TRIGGER_A].setPrefix(settings.outputAddress[3], ",i");
  messages[TRIGGER_B].setPrefix(settings.outputAddress[4], ",i");
#ifdef SERVICE_BUS
  messages[PARAMETER_AA].setPrefix("/osm/aa", ",i");
  messages[PARAMETER_AB].setPrefix("/osm/ab", ",i");
#endif
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
