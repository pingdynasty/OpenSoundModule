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
#ifdef SERVICE_BUS
#include "DigitalBusReader.h"
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
#ifdef SERVICE_BUS
DigitalBusReader bus;
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
#ifdef SERVICE_BUS
  out.print("Digital Bus UID["); 
  out.print(bus.getUid());
  out.print("]: ");
  out.print(bus.getPeers());
  out.println(" peers"); 
#endif
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

// WebSocketServer websocketserver(WEBSOCKET_SERVER_PORT);
// TcpSocketServer tcpsocketserver(TCP_SERVER_PORT);

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
  debugMessage("oscserver.begin");
  success = oscserver.begin(settings.localPort);
  if(success)
    debugMessage("oscserver success");
  else
    debugMessage("oscserver fail");
#ifdef SERVICE_MDNS
  debugMessage("mdns.begin");
  success = mdns.begin();
  if(success)
    debugMessage("mdns success");
  else
    debugMessage("mdns fail");
#endif
#ifdef SERIAL_DEBUG
  debugMessage("startServers done");
  printInfo(Serial);
#endif
#ifdef SERVICE_BUS
  bus.startDiscover();
  debugMessage("started digital bus");
#endif /* SERVICE_BUS */
}

void stopServers(){
  debugMessage("stopServers");
  if(connection.getCurrentNetwork() == -1)
    return;
  //  tcpsocketserver.stop();
  //  websocketserver.stop();
  debugMessage("webserver.stop");
  webserver.stop();
  debugMessage("oscserver.stop");
  oscserver.stop();
#ifdef SERVICE_MDNS
  debugMessage("mdns.stop");
  mdns.stop();
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
#ifdef SERVICE_BUS
  Serial1.begin(DIGITAL_BUS_BAUD);
  debugMessage("begin digital bus");
#endif
  debugMessage("setup complete");
}

void process();
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
    process();
  }
#ifdef SERIAL_CONSOLE
  processConsole(Serial);
#endif
  processButton();
#ifdef SERVICE_BUS
  while(Serial1.available() >= 4){
    uint8_t buf[4];
    buf[0] = (uint8_t)Serial1.read();
    buf[1] = (uint8_t)Serial1.read();
    buf[2] = (uint8_t)Serial1.read();
    buf[3] = (uint8_t)Serial1.read();
    bus.readBusFrame(buf);
  }
#endif
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
#ifdef SERVICE_BUS
      bool bussed = bus.connected();
      if(bussed)
	bus.startIdent();
      debug << "Bus [" << bussed << "][" << bus.getUid() << "][" << bus.getNuid() << "][" << bus.getPeers() << "]\r\n";
#endif
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

void process(){
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
  //  websocketserver.loop();
  //  tcpsocketserver.loop();
  oscserver.loop();
// #ifdef SERVICE_BUS
//   bus.connected();
// #endif
}

void reload(){
  oscserver.stop();
  configureOsc();
  oscserver.begin(settings.localPort);
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

#ifdef SERVICE_BUS
/* outgoing: send message over digital bus */
void bus_tx_parameter(uint8_t pid, int16_t value){
  debug << "tx parameter [" << pid << "][" << value << "]\r\n" ;
  bus.sendParameterChange(pid, value);
}

/* incoming: callback when message received on digital bus */
void bus_rx_parameter(uint8_t pid, int16_t value){
  debug << "rx parameter [" << pid << "][" << value << "]\r\n" ;
  // oscsender.sendFloat((OscSender::OscMessageId)(OscSender::PARAMETER_AA+pid), value/4096.0f);
  oscsender.sendFloat((OscSender::OscMessageId)pid, value/4096.0f);
}

void bus_tx_error(const char* reason){
  debug << "Digital bus send error: " << reason << ".\r\n";
}

void bus_rx_error(const char* reason){
  debug << "Digital bus receive error: " << reason << ".\r\n";
  debug << "Discarding " << Serial1.available() << " bytes.\r\n";
  while(Serial1.available())
    Serial1.read();
}

#endif

#include "console.h"
