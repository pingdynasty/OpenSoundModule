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
  sprintf(buf, "%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], settings.localPort);
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

void oscLed(OscServer& server, OscMessage& msg){
  debugMessage("osc led");
  if(msg.getSize() == 0)
    toggleLed();
  else
    setLed(msg.getAsBool(0) ? LED_GREEN : LED_YELLOW);
}

// #ifdef SERVICE_BUS
// #include "OpenWareMidiControl.h"
// void oscAA(OscServer& server, OscMessage& msg){
//   debugMessage("osc aa");
//   float value = msg.getAsFloat(0);
//   rxParameter(PARAMETER_AA, max(INT16_MIN, min(INT16_MAX, (int32_t)(value*4096))));
// }

// void oscAB(OscServer& server, OscMessage& msg){
//   debugMessage("osc ab");
//   float value = msg.getAsFloat(0);
//   rxParameter(PARAMETER_AB, value*4096);
// }
// #endif

uint16_t scaleInputValue(int def, float value){
  value = (value - settings.min[def])/(settings.max[def] - settings.min[def]);
  return MIN(MAX(int(4095.0f*value), 0), 4095);
}

float scaleOutputValue(int def, uint16_t value){
  float out = value/4095.0f;
  out = out*(settings.max[def] - settings.min[def]) + settings.min[def];
  return out;
//  return MIN(MAX(out, settings.min[def]), settings.max[def]);
}

void oscCv(OscServer& server, OscMessage& msg){
  float a = msg.getAsFloat(0);
  setCVA(scaleInputValue(CV_A_IN, a));
  float b = msg.getAsFloat(1);
  setCVB(scaleInputValue(CV_B_IN, b));
  debug << "osc cv: " << a << "/" << b << "\r\n";
}

void oscCvA(OscServer& server, OscMessage& msg){
  float value = msg.getAsFloat(0);
  debug << "osc cv a: " << value << "\r\n";
  setCVA(scaleInputValue(CV_A_IN, value));
}

void oscCvB(OscServer& server, OscMessage& msg){
  float value = msg.getAsFloat(0);
  debug << "osc cv b: " << value << "\r\n";
  setCVB(scaleInputValue(CV_B_IN, value));
}

void oscTriggerA(OscServer& server, OscMessage& msg){
  debugMessage("osc trigger a");
  if(msg.getSize() == 0)
    toggleTriggerA();
  else
    setTriggerA(msg.getAsBool(0));
}

void oscTriggerB(OscServer& server, OscMessage& msg){
  debugMessage("osc trigger b");
  if(msg.getSize() == 0)
    toggleTriggerB();
  else
    setTriggerB(msg.getAsBool(0));
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
  oscserver.addCommand(settings.inputAddress[0], &oscStatus);
  oscserver.addCommand(settings.inputAddress[1], &oscCvA, 1);
  oscserver.addCommand(settings.inputAddress[2], &oscCvB, 1);
  oscserver.addCommand(settings.inputAddress[3], &oscTriggerA);
  oscserver.addCommand(settings.inputAddress[4], &oscTriggerB);
  oscserver.addCommand("/osm/cv", &oscCv, 2);
  oscserver.addCommand("/osm/led", &oscLed);
}
