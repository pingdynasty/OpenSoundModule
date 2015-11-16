#include "application.h"
#include "OscSender.h"
#include "OscServer.h"
#include "ApplicationSettings.h"

extern OscServer oscserver;

OscSender::OscSender(){
}

void OscSender::init(){
  messages[STATUS].setAddress(addressSettings.outputAddress[0]);
  messages[STATUS].addString();
  messages[CV_A].setAddress(addressSettings.outputAddress[1]);
  messages[CV_A].addFloat(.0f);
  messages[CV_B].setAddress(addressSettings.outputAddress[2]);
  messages[CV_B].addFloat(.0f);
  messages[TRIGGER_A].setAddress(addressSettings.outputAddress[3]);
  messages[TRIGGER_A].addInt(0);
  messages[TRIGGER_B].setAddress(addressSettings.outputAddress[4]);
  messages[TRIGGER_B].addInt(0);
    // osc_status_msg.addString();
    // osc_a_cv_msg.addFloat(.0f);
    // osc_b_cv_msg.addFloat(.0f);
    // osc_a_trigger_msg.addInt(0);
    // osc_b_trigger_msg.addInt(0);
  // note_on_msg.add((int32_t)0);
  // note_on_msg.add((int32_t)0);
  // note_on_msg.add((int32_t)0);
  // note_off_msg.add((int32_t)0);
  // note_off_msg.add((int32_t)0);
  // control_change_msg.add((int32_t)0);
  // control_change_msg.add((int32_t)0);
  // control_change_msg.add((int32_t)0);
  // pitch_bend_msg.add((int32_t)0);
  // pitch_bend_msg.add((int32_t)0);
  }


void OscSender::send(OscMessageId mid, int value){
  if(mid < MESSAGE_COUNT){ 
    messages[mid].setInt(0, value);
    oscserver.beginPacket();
    messages[mid].write(oscserver);
    oscserver.endPacket();
  }
}

void OscSender::send(OscMessageId mid, float value){
  if(mid < MESSAGE_COUNT){
    messages[mid].setFloat(0, value);
    oscserver.beginPacket();
    messages[mid].write(oscserver);
    oscserver.endPacket();
  }
}

void OscSender::send(OscMessageId mid, const char* value){
  if(mid < MESSAGE_COUNT){
    messages[mid].setString(0, value);
    oscserver.beginPacket();
    messages[mid].write(oscserver);
    oscserver.endPacket();
  }
}
