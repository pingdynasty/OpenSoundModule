#include "opensound.h"
#include "OscMessage.hpp"
#include "OscSender.h"
#include "OscServer.h"

OscServer oscserver;
OscSender oscsender;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

void sendOscStatus(const char* status){
  debugMessage("sending osc status");
  debugMessage(status);
  oscsender.send(OscSender::STATUS, status);
  //  osc_status_msg.setString(0, status);
  //  oscserver.sendMessage(osc_status_msg);
}

void broadcastStatus(){
  bool broadcast = oscserver.isBroadcastMode();
  oscserver.setBroadcastMode(true);
  IPAddress ip = WiFi.localIP();
  char buf[24];
  sprintf(buf, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], networkSettings.localPort);
  sendOscStatus(buf);
  oscserver.setBroadcastMode(broadcast);
}

void oscStatus(OscServer& server, OscMessage& msg){
  debugMessage("osc status");
  if(msg.getDataType(0) == 's')
    debugMessage(msg.getString(0));
  sendOscStatus("hi");
  broadcastStatus();
}

float getFloatValue(OscMessage msg, int i){
  float f;
  switch(msg.getDataType(i)){
  case 'f':
    f = msg.getFloat(i);
    break;
  case 'i':
    f = float(msg.getInt(i));
    break;
  case 'd':
    f = float(msg.getDouble(i));
    break;
  case 'h':
    f = float(msg.getLong(i));
    break;
  case 'T':
    f = 1.0f;
    break;
  default:
    f = 0.0f;
    break;
  }
  return f;
}

bool getBoolValue(OscMessage msg, int i){
  bool b;
  switch(msg.getDataType(i)){
  case 'f':
    b = msg.getFloat(i) > 0.5;
    break;
  case 'i':
    b = msg.getInt(i) != 0;
    break;
  case 'd':
    b = msg.getDouble(i) > 0.5;
    break;
  case 'h':
    b = msg.getLong(i) != 0;
    break;
  case 'T':
    b = true;
    break;
  default:
    b = false;
    break;
  }
  return b;
}

void oscLed(OscServer& server, OscMessage& msg){
  debugMessage("osc led");
  if(msg.getSize() == 0)
    toggleLed();
  else
    setLed(getBoolValue(msg, 0) ? LED_GREEN : LED_YELLOW);
}

uint16_t scaleInputValue(int def, float value){
  value = (value - rangeSettings.min[def])/(rangeSettings.max[def] - rangeSettings.min[def]);
  return MIN(MAX(int(4095.0f*value), 0), 4095);
}

float scaleOutputValue(int def, uint16_t value){
  float out = value/4095.0f;
  out = out*(rangeSettings.max[def] - rangeSettings.min[def]) + rangeSettings.min[def];
  return out;
//  return MIN(MAX(out, rangeSettings.min[def]), rangeSettings.max[def]);
}

void oscCv(OscServer& server, OscMessage& msg){
  float a = getFloatValue(msg, 0);
  setCVA(scaleInputValue(CV_A_IN, a));
  float b = getFloatValue(msg, 1);
  setCVB(scaleInputValue(CV_B_IN, b));
  debug << "osc cv: " << a << "/" << b << "\r\n";
}

void oscCvA(OscServer& server, OscMessage& msg){
  float value = getFloatValue(msg, 0);
  debug << "osc cv a: " << value << "\r\n";
  setCVA(scaleInputValue(CV_A_IN, value));
}

void oscCvB(OscServer& server, OscMessage& msg){
  float value = getFloatValue(msg, 0);
  debug << "osc cv b: " << value << "\r\n";
  setCVB(scaleInputValue(CV_B_IN, value));
}

void oscTriggerA(OscServer& server, OscMessage& msg){
  debugMessage("osc trigger a");
  if(msg.getSize() == 0)
    toggleTriggerA();
  else
    setTriggerA(getBoolValue(msg, 0));
}

void oscTriggerB(OscServer& server, OscMessage& msg){
  debugMessage("osc trigger b");
  if(msg.getSize() == 0)
    toggleTriggerB();
  else
    setTriggerB(getBoolValue(msg, 0));
}

void sendCvA(uint16_t value){
  oscsender.send(OscSender::CV_A, scaleOutputValue(CV_A_OUT, value));
}

void sendCvB(uint16_t value){
  oscsender.send(OscSender::CV_B, scaleOutputValue(CV_B_OUT, value));
}

void sendTriggerA(bool value){
  oscsender.send(OscSender::TRIGGER_A, (int)value);
}

void sendTriggerB(bool value){
  oscsender.send(OscSender::TRIGGER_B, (int)value);
}

void configureOsc(){
  oscsender.init();
  oscserver.init();
  oscserver.addCommand(addressSettings.inputAddress[0], &oscStatus);
  oscserver.addCommand(addressSettings.inputAddress[1], &oscCvA, 1);
  oscserver.addCommand(addressSettings.inputAddress[2], &oscCvB, 1);
  oscserver.addCommand(addressSettings.inputAddress[3], &oscTriggerA);
  oscserver.addCommand(addressSettings.inputAddress[4], &oscTriggerB);
  oscserver.addCommand("/osm/cv", &oscCv, 2);
  oscserver.addCommand("/osm/led", &oscLed);
}
