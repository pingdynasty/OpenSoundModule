#include "opensound.h"
#ifdef SERIAL_CONSOLE

#include "console.h"
#include "ConnectionManager.h"
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

void processConsole(Stream& port){
  static bool unlocked = false;
  if(port.available() > 0){
    int c = port.read();
    if(!unlocked && c != '$')
      return;
    unlocked = true;
    switch(c){
    case '?':
      port.println("available commands:");
      port.println("?: print help");
      port.println("b: broadcast status");
      port.println("c: clear credentials");
      port.println("f: factory reset");
      port.println("i: print device info");
      port.println("l: print local IP address");
      port.println("s: scan wifi");
      port.println("w: add wifi credentials");
      break;
    case 'b':
      port.println("b: broadcast status");
      broadcastStatus();
      break;
    case 'c':
      port.println("c: clear credentials");
      connection.clearCredentials();
      printInfo(port);
      break;
    case 'f':
      port.println("f: factory reset");
      factoryReset();
      break;
    case 'i':
      port.println("i: print device info");
      printInfo(port);
      break;
    case 'l': {
      port.println("l: print local IP address");
      port.println(connection.getLocalIPAddress());
      break;
    case 's':
      port.println("s: scan wifi");
      scanner.start(&port);
      break;
    case 'w':
      port.println("w: add wifi credentials");
      readCredentials(port);
      break;
    }
    case ':': {
      port.println("System Admin");
      while(port.available() < 1); // wait
      c = port.read();
      switch(c){
      case 'a':
	port.println("a: access point connect");
	connection.connect(NETWORK_ACCESS_POINT);
	break;
      case 'w':
	port.println("w: wifi connect");
	connection.connect(NETWORK_LOCAL_WIFI);
	break;
      case '=':
	port.println("=: set access point credentials");
	readAccessPointCredentials(port);
	break;
      case 'g':
	port.println("g: generate credentials");
	connection.generateAccessPointCredentials(port);
	break;
      }
      break;
    }
      // calibration commands
    case '[':
      port.println("[: dac 0");
      setCVA(0);
      setCVB(0);
      break;
    case '|':
      port.println("|: dac 1/2");
      setCVA(2047);
      setCVB(2047);
      break;
    case ']':
      port.println("]: dac full");
      setCVA(4095);
      setCVB(4095);
      break;
    default:
      port.println("unknown command (? for help)");
      break;
    }
  }
}

#endif /* SERIAL_CONSOLE */
