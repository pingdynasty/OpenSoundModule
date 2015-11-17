#ifdef SERIAL_CONSOLE
#include "Scanner.hpp"
Scanner scanner;

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

void processSerial(){
  static bool unlocked = false;
  if(Serial.available() > 0){
    int c = Serial.read();
    if(!unlocked && c != '$')
      return;
    unlocked = true;
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
      case 'g':
	debugMessage("g: generate credentials");
	connection.generateAccessPointCredentials();
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
