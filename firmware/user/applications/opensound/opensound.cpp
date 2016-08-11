// #include <assert.h> causes undefined symbols in nanolib
#include "application.h"
#include "opensound.h"
#include "osc.hpp"
#include "web.hpp"
#include <stdint.h>
// #include "TcpSocketServer.hpp"
// #include "WebServer.hpp"
#include "dac.h"
#include "ApplicationSettings.h"
#include "ConnectionManager.h"

#ifdef SERVICE_MDNS
#include "mdns/MDNS.h"
#endif

#ifdef SERVICE_WEBSOCKETS
#include "WebSocketServer.hpp"
#define WEBSOCKET_SERVER_PORT 8008
WebSocketServer websocketserver(WEBSOCKET_SERVER_PORT);
#endif

SYSTEM_MODE(MANUAL);
// #define RX_BUFFER_LENGTH 64
#define TCP_SERVER_PORT       8888
#define HTTP_SERVER_PORT      80
#define WEBSOCKET_SERVER_PORT 8008
#define SERIAL_BAUD_RATE      57600
#define DEFAULT_ANTENNA       ANT_AUTO
#define BUTTON_DEBOUNCE_MS    100
#define BUTTON_TOGGLE_MS      2000

ConnectionManager connection;
#ifdef SERVICE_MDNS
MDNS mdns;
#endif

void printInfo(Print& out){
  out.println("Device Status");
  if(connection.isWiFiConnected())
    out.println("WiFi Connected");
  if(connection.isIpConnected())
    out.println("IP Connected");
  /*
  if(WiFi.connecting())
    out.println("Connecting");
  if(WiFi.listening())
    out.println("Listening");
  if(WiFi.ready())
    out.println("Ready");
  if(WiFi.hasCredentials())
    out.println("Has Credentials");
  */
  out.print("Device ID: "); 
  out.println(Particle.deviceID());
  out.print("SSID: "); 
  out.println(connection.getSSID());
  out.print("Local IP: "); 
  out.println(connection.getLocalIPAddress());
  out.print("Gateway: "); 
  out.println(connection.getDefaultGateway());
  out.print("RSSI: "); 
  out.println(connection.getRSSI());
  out.print("Local port: "); 
  out.println(settings.localPort);
  out.print("Remote IP: "); 
  out.println(oscserver.remoteIP());
  out.print("Remote Port: "); 
  out.println(oscserver.remotePort());
  out.print("MAC Address: ");
  connection.printMacAddress(out);
  out.println();
  /*
  out.print("Hostname: "); 
  out.println(connection.getHostname());
  */
  out.print("Accesspoint: "); 
  out.println(connection.getAccessPointSSID());
  out.print("Free memory: "); 
  out.println(System.freeMemory());
}

void setLed(LedPin led){
  switch(led){
  case LED_YELLOW:
    digitalWrite(YELLOW_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    break;
  case LED_GREEN:
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    break;
  case LED_NONE:
    digitalWrite(YELLOW_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, LOW);
    break;
  }
}

void toggleLed(){
  digitalWrite(YELLOW_LED_PIN, !digitalRead(YELLOW_LED_PIN));
  digitalWrite(GREEN_LED_PIN, !digitalRead(GREEN_LED_PIN));
}

void setRemoteIpAddress(const char* address){
  String ip(address);
#ifdef SERIAL_DEBUG
  Serial.print("Set remote IP: ");
  Serial.println(ip);
#endif
  ip.toLowerCase();
  if(ip.startsWith("auto")){
    oscserver.autoRemoteIPAddress = true;
    return;
  }
  if(ip.equals("broadcast")){
    oscserver.autoRemoteIPAddress = false;
    oscserver.setBroadcastMode(true);
    return;
  }
  oscserver.autoRemoteIPAddress = false;
  int pos = 0;
  int idx = ip.indexOf('.');
  for(int i=0; i<3; ++i){
    oscserver.remoteIPAddress[i] = ip.substring(pos, idx).toInt();
    pos = idx+1;
    idx = ip.indexOf('.', pos);
  }
  oscserver.remoteIPAddress[3] = ip.substring(pos).toInt();
  settings.remoteIPAddress = oscserver.remoteIPAddress;
#ifdef SERIAL_DEBUG
  Serial.print("Remote IP: ");
  Serial.println(oscserver.remoteIPAddress);
#endif
}

void startServers(){
  debugMessage("start servers");
  /*
  if(!WiFi.ready())
    debugMessage("wifi not ready");
  if(WiFi.connecting())
    debugMessage("wifi connecting");
  */
  configureWeb();
  configureOsc();
  debugMessage("webserver.begin");
  bool success = webserver.begin();
  if(success)
    debugMessage("webserver success");
  else
    debugMessage("webserver fail");
#ifdef SERVICE_OSC
  debugMessage("oscserver.begin");
  success = oscserver.begin(settings.localPort);
  if(success)
    debugMessage("oscserver success");
  else
    debugMessage("oscserver fail");
#endif
#ifdef SERVICE_MDNS
  debugMessage("mdns.begin");
  success = mdns.begin();
  if(success)
    debugMessage("mdns success");
  else
    debugMessage("mdns fail");
#endif
#ifdef SERVICE_WEBSOCKETS
  websocketserver.begin();
#endif
#ifdef SERIAL_DEBUG
  debugMessage("startServers done");
  printInfo(Serial);
#endif
}

void stopServers(){
  debugMessage("stopServers");
  if(connection.getCurrentNetwork() == -1)
    return;
  debugMessage("webserver.stop");
  webserver.stop();
#ifdef SERVICE_OSC
  debugMessage("oscserver.stop");
  oscserver.stop();
#endif
#ifdef SERVICE_MDNS
  debugMessage("mdns.stop");
  mdns.stop();
#endif
#ifdef SERVICE_WEBSOCKETS
  websocketserver.stop();
#endif
  debugMessage("stopServers done");
}

unsigned long lastButtonPress;
uint16_t cvA, cvB;
bool triggerA, triggerB;
bool button;

//IntervalTimer dacTimer;
static uint16_t cvOutA = 2047;
static uint16_t cvOutB = 2047;
uint16_t getCVA(){
  return cvOutA;
}

uint16_t getCVB(){
  return cvOutB;
}

void setCVA(uint16_t cv){
#ifdef SERIAL_DEBUG
  Serial.print("cv A: ");
  Serial.println(cv);
#endif
  cvOutA = cv;
}

void setCVB(uint16_t cv){
#ifdef SERIAL_DEBUG
  Serial.print("cv B: ");
  Serial.println(cv);
#endif
  cvOutB = cv;
}

uint16_t smooth = 3; 
void dacCallback(){
  static uint16_t a = 0;
  static uint16_t b = 0;
  a = (a*smooth + cvOutA)/(smooth+1);
  //  dac_set_a(a);
  b = (b*smooth + cvOutB)/(smooth+1);
  dac_set_ab(a, b);
}

//Timer dacTimer(50, dacCallback());

void setTriggerA(int value){
  digitalWrite(DIGITAL_OUTPUT_PIN_A, value == 0 ? HIGH : LOW);
#ifdef SERIAL_DEBUG
  Serial.print("trigger A: ");
  Serial.println(value);
#endif
}

void setTriggerB(int value){
  digitalWrite(DIGITAL_OUTPUT_PIN_B, value == 0 ? HIGH : LOW);
#ifdef SERIAL_DEBUG
  Serial.print("trigger B: ");
  Serial.println(value);
#endif
}

void toggleTriggerA(){
  bool value = digitalRead(DIGITAL_OUTPUT_PIN_A);
  setTriggerA(value);
}

void toggleTriggerB(){
  bool value = digitalRead(DIGITAL_OUTPUT_PIN_B);
  setTriggerB(value);
}

bool isButtonPressed(){
  //  return false;
  return !digitalRead(BUTTON_PIN);
}

void setup(){
  setLed(LED_GREEN);
  pinMode(ANALOG_PIN_A, INPUT);
  pinMode(ANALOG_PIN_B, INPUT);
  pinMode(DIGITAL_INPUT_PIN_A, INPUT);
  pinMode(DIGITAL_INPUT_PIN_B, INPUT);
  pinMode(DIGITAL_OUTPUT_PIN_A, OUTPUT);
  pinMode(DIGITAL_OUTPUT_PIN_B, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  setTriggerA(0);
  setTriggerB(0);
  WiFi.on();
  randomSeed(micros());

#if defined SERIAL_DEBUG || defined SERIAL_CONSOLE
  Serial.begin(SERIAL_BAUD_RATE);
  debugMessage("Serial.begin");
#endif

  settings.init();

  dac_init();
  debugMessage("dac init");

  // WiFi.selectAntenna(DEFAULT_ANTENNA);
  wwd_wifi_select_antenna(WICED_ANTENNA_AUTO);  

  // Called once at startup to initialize the wlan hardware.
  //wlan_setup();
  //  wlan_activate();
  debugMessage("wifi.on");

  if(WiFi.hasCredentials())
    connection.connect(NETWORK_LOCAL_WIFI);
  else
    connection.connect(NETWORK_ACCESS_POINT);

  lastButtonPress = 0;
  button = isButtonPressed();
  cvA = analogRead(ANALOG_PIN_A);
  cvB = analogRead(ANALOG_PIN_B);
  
#ifdef SERVICE_MDNS
  debugMessage("mdns setup");
  const char* hostname = getDeviceName();
  if(hostname == NULL)
    hostname = OSM_AP_HOSTNAME;
  uint16_t port = settings.localPort;
  mdns.setHostname("osm");
  mdns.setService("udp", "osc", port, hostname);
  // success &= mdns.addTXTEntry("port", "1");
#endif /* SERVICE_MDNS */
  //  dacTimer.begin(dacCallback, 400, hmSec);
  // dacTimer.start();
  debugMessage("setup complete");
}

void processInputs();
void processButton();
#ifdef SERIAL_CONSOLE
void processConsole(Stream& port);
#endif

void loop(){
  dacCallback();
  if(connection.connected()){
#ifdef SERVICE_MDNS
    mdns.processQueries();
#endif
#ifdef SERVICE_WEBSOCKETS
    websocketserver.loop();
#endif
    processInputs();
#ifdef SERVICE_OSC
    oscserver.loop();
#endif
  }
#ifdef SERIAL_CONSOLE
  processConsole(Serial);
#endif
  processButton();
}

void processButton(){
  bool btn = isButtonPressed();
  if(btn != button && (millis() > lastButtonPress+BUTTON_DEBOUNCE_MS)){
    button = btn;
    setLed(connection.getCurrentNetwork() == NETWORK_LOCAL_WIFI ? LED_GREEN : LED_YELLOW);
    if(button){
      lastButtonPress = millis();
      toggleLed();
      broadcastStatus();
    }else{
      lastButtonPress = 0;
    }    
  }
  if(lastButtonPress && (millis() > lastButtonPress+BUTTON_TOGGLE_MS*2)){
    debugMessage("toggle network");
    connection.disconnect();
    if(connection.getCurrentNetwork() == NETWORK_ACCESS_POINT 
       && WiFi.hasCredentials())
      connection.connect(NETWORK_LOCAL_WIFI);
    else
      connection.connect(NETWORK_ACCESS_POINT);
    lastButtonPress = 0; // prevent retrigger
  }else if(lastButtonPress && (millis() > lastButtonPress+BUTTON_TOGGLE_MS)){
    setLed(connection.getCurrentNetwork() == NETWORK_LOCAL_WIFI ? LED_GREEN : LED_YELLOW);
  }
}

void processInputs(){
  int cv = analogRead(ANALOG_PIN_A);
  if(abs(cv - cvA) > ANALOG_THRESHOLD){
    cvA = cv;
    sendCvA(4095-cvA);
  }
  cv = analogRead(ANALOG_PIN_B);
  if(abs(cv - cvB) > ANALOG_THRESHOLD){
    cvB = cv;
    sendCvB(4095-cvB);
  }
  bool btn = !digitalRead(DIGITAL_INPUT_PIN_A);
  if(btn != triggerA){
    triggerA = btn;
    sendTriggerA(btn);
  }
  btn = !digitalRead(DIGITAL_INPUT_PIN_B);
  if(btn != triggerB){
    triggerB = btn;
    sendTriggerB(btn);
  }
}

void reload(){
#ifdef SERVICE_OSC
  oscserver.stop();
  configureOsc();
  oscserver.begin(settings.localPort);
#endif
}

void factoryReset(){
#ifdef SERIAL_DEBUG
  if(settings.settingsInFlash())
    debugMessage("settings in flash");
#endif
  settings.reset();
  if(settings.settingsInFlash())
    settings.clearFlash();
  if(connection.hasCredentials())
    connection.clearCredentials();
  connection.setAccessPointPrefix(OSM_AP_HOSTNAME);
  
  //  connection.setHostname(OSM_AP_HOSTNAME);
/*
  connection.setAccessPointPrefix(OSM_AP_HOSTNAME);
  connection.setAccessPointCredentials(OSM_AP_SSID, OSM_AP_PASSWD, OSM_AP_AUTH);
*/
  connection.connect(NETWORK_ACCESS_POINT);
}

const char* getDeviceName(){
  return connection.getAccessPointPrefix();
}

void setDeviceName(const char* name){
  connection.setAccessPointPrefix(name);
}

#include "console.h"
