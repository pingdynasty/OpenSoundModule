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

#define OSM_AP_SSID                   "OpenSoundModule"
#define OSM_AP_PASSWD                 "dadac0de"
#define OSM_AP_AUTH                   "3"
#define OSM_AP_HOSTNAME               "OpenSoundModule"

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
  out.println(networkSettings.localPort);
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
  networkSettings.remoteIPAddress = oscserver.remoteIPAddress;
#ifdef SERIAL_DEBUG
  Serial.print("Remote IP: ");
  Serial.println(oscserver.remoteIPAddress);
#endif
}

void readCredentials(Stream& port){
  port.setTimeout(10000);
  port.println("Enter SSID:");
  String ssid = port.readStringUntil('\r');
  ssid.trim();
  port.println("Enter AP security (0=Open, 1=WEP, 2=WPA, 3=WPA2):");
  String auth = port.readStringUntil('\r');
  auth.trim();
  port.println("Enter password:");
  String pass = port.readStringUntil('\r');
  pass.trim();
  port.print("SSID: [");
  port.print(ssid);
  port.print("] Auth: [");
  port.print(auth);
  port.print("] Password: [");
  port.print(pass);
  port.println("]");
  port.println("Type yes to confirm");
  String yes = port.readStringUntil('\r');
  port.setTimeout(1000);
  if(yes.equals("yes"))
    connection.setCredentials(ssid.c_str(), pass.c_str(), auth.c_str());
  else
    port.println("Cancelled");
}

void readAccessPointCredentials(Stream& port){
  port.setTimeout(10000);
  port.println("Enter AP SSID:");
  String ssid = port.readStringUntil('\r');
  ssid.trim();
  port.println("Enter AP security (0=Open, 1=WEP, 2=WPA, 3=WPA2):");
  String auth = port.readStringUntil('\r');
  auth.trim();
  port.println("Enter AP password:");
  String pass = port.readStringUntil('\r');
  pass.trim();
  port.print("SSID: [");
  port.print(ssid);
  port.print("] Auth: [");
  port.print(auth);
  port.print("] Password: [");
  port.print(pass);
  port.println("]");
  port.println("Type yes to confirm");
  String yes = port.readStringUntil('\r');
  port.setTimeout(1000);
  if(yes.equals("yes")){
    connection.setAccessPointCredentials(ssid.c_str(), pass.c_str(), auth.c_str());
    port.println("Done");
  }else
    port.println("Cancelled");
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
  success = oscserver.begin(networkSettings.localPort);
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
  WiFi.on();

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

#ifdef SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD_RATE);
  //  Serial1.begin(SERIAL_BAUD_RATE);
  debugMessage("Serial.go");
  //  Serial1.print("Serial1.go");
#endif

  networkSettings.init();
  addressSettings.init();
  rangeSettings.init();

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
  debugMessage("mdns");
#if 0
    bool success = mdns.setHostname("core-1");
    success &= mdns.setService("tcp", "http", 80, "Core 1");
    success &= mdns.addTXTEntry("coreid", "1");
#else
  // mdns
  bool success = mdns.setHostname("osm");
  success &= mdns.setService("udp", "osc", 8000, OSM_AP_HOSTNAME);
  //success &= mdns.setService("tcp", "http", 80, OSM_AP_HOSTNAME);
  success &= mdns.addTXTEntry("coreid", "1");
#endif
  if(success)
    debugMessage("mdns config succeeded");
  else
    debugMessage("mdns config failed");  
  debugMessage("mdns done");
#endif /* SERVICE_MDNS */

  //  dacTimer.begin(dacCallback, 400, hmSec);
  // dacTimer.start();
}

void process();
void processButton();
#ifdef SERIAL_CONSOLE
void processSerial();
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
  processSerial();
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
  // websocketserver.loop();
  //  tcpsocketserver.loop();
  oscserver.loop();
}

#include "Scanner.hpp"
Scanner scanner;

#ifdef SERIAL_CONSOLE
void processSerial(){
  if(Serial.available() > 0){
    int c = Serial.read();
    switch(c){
    case 'f':
      debugMessage("f: factory reset");
      factoryReset();
      break;
    case '?':
      debugMessage("?: print info");
      printInfo(Serial);
      break;
    case 's':
      debugMessage("s: scan wifi");
      scanner.start();
      break;
    case '!':
      debugMessage("!: clear credentials");
      connection.clearCredentials();
      printInfo(Serial);
      break;
    case 'b':
      debugMessage("b: broadcast mode");
      oscserver.setBroadcastMode(true);
      break;
    case '+':
      debugMessage("+: add credentials");
      readCredentials(Serial);
      break;
    case '=':
      debugMessage("=: set access point credentials");
      readAccessPointCredentials(Serial);
      break;
    case 'l':
      debugMessage("l: toggle led");
      toggleLed();
      break;
    case '0':
      debugMessage("0: internal antenna");
      WiFi.selectAntenna(ANT_INTERNAL);
      break;
    case '1':
      debugMessage("1: auto antenna");
      WiFi.selectAntenna(ANT_AUTO);
      break;
    case '2':
      debugMessage("2: external antenna");
      WiFi.selectAntenna(ANT_EXTERNAL);
      break;
    case '<':
      debugMessage("<: stop servers");
      stopServers();
      break;
    case '>':
      debugMessage(">: start servers");
      startServers();
      break;
    case 'a':
      debugMessage("a: access point connect");
      connection.connect(NETWORK_ACCESS_POINT);
      break;
    case 'w':
      debugMessage("w: wifi connect");
      connection.connect(NETWORK_LOCAL_WIFI);
      break;
    case '*': {
      debugMessage("*: print local IP address");
      Serial.println(connection.getLocalIPAddress());
      break;
    }
    case ':': {
      debugMessage("System Admin");
      while(Serial.available() < 1); // wait
      c = Serial.read();
      switch(c){      
      case '+':
	debugMessage("+: activate wlan");
	wlan_activate();
	break;
      case '-':
	debugMessage("-: deactivate wlan");
	wlan_deactivate();
	break;
      case '0':
	debugMessage("0: select STA");
	wlan_select_interface(NETWORK_LOCAL_WIFI);
	break;
      case '1':
	debugMessage("1: select AP");
	wlan_select_interface(NETWORK_ACCESS_POINT);
	break;
      case 'i':
	debugMessage("i: connect init");
	wlan_connect_init();
	break;
      case 'f':
	debugMessage("f: connect finalise");
	wlan_connect_finalize();
	break;
      case 'd':
	debugMessage("d: disconnect");
	wlan_disconnect_now();
	break;
      case '>':
	debugMessage(">: start DNS");
	WiFi.startDNS();
	break;
      case '<':
	debugMessage("<: stop DNS");
	WiFi.stopDNS();
	break;
      case 'o':
	debugMessage("o: WiFi.on");
	WiFi.on();
	break;
      case 'w':
	debugMessage("w: WiFi.connect");
	WiFi.connect();
	break;
      case 'x':
	debugMessage("x: WiFi.disconnect");
	WiFi.disconnect();
	break;
      }
      break;
    }
    case '[':
      debugMessage("[: dac 0");
      setCVA(0);
      setCVB(0);
      dac_set_ab(0, 0);
      break;
    case '|':
      debugMessage("|: dac 1/2");
      setCVA(2047);
      setCVB(2047);
      dac_set_ab(2047, 2047);
      break;
    case ']':
      debugMessage("]: dac full");
      setCVA(4095);
      setCVB(4095);
      dac_set_ab(4095, 4095);
      break;
    }
  }
}
#endif /* SERIAL_CONSOLE */

void reload(){
  oscserver.stop();
  configureOsc();
  oscserver.begin(networkSettings.localPort);
}

void factoryReset(){
  networkSettings.reset();
  networkSettings.clearFlash();
  addressSettings.reset();
  addressSettings.clearFlash();
  connection.clearCredentials();
  //  connection.setHostname(OSM_AP_HOSTNAME);
  connection.setAccessPointPrefix(OSM_AP_HOSTNAME);
  connection.setAccessPointCredentials(OSM_AP_SSID, OSM_AP_PASSWD, OSM_AP_AUTH);
  connection.connect(NETWORK_ACCESS_POINT);
}

const char* getDeviceName(){
  return connection.getAccessPointSSID();
}

void setDeviceName(const char* name){
  connection.setAccessPointPrefix(name);
}
