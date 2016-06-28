#include "web.hpp"
#include "opensound.h"
#include "application.h"
#include "ApplicationSettings.h"
#include "ConnectionManager.h"

WebServer webserver;

void configureWeb(){
}

#ifdef SERVICE_BUS
#include "DigitalBusReader.h"
extern DigitalBusReader bus;
#endif

int32_t process_status(const char* url, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){		       
  Streamer stream(s);
  stream << OSM_BEGIN << "<h1>Status</h1><h2>Open Sound Module</h2>";
  /*
  if(connection.isWiFiConnected())
    stream << "<p>WiFi Connected</p>";
  if(connection.isIpConnected())
    stream << "<p>Network up</p>";
  if(WiFi.connecting())
    stream << "<p>Connecting</p>";
  if(WiFi.listening())
    stream << "<p>Listening</p>";
  */
  if(WiFi.ready())
    stream << "<p>Ready!</p>";
#ifdef SERVICE_BUS
  stream << "<p>Digital Bus: " << bus.getPeers() << " peers</p>";
#endif
  /*
  if(WiFi.hasCredentials())
    stream << "<p>WiFi credentials stored</p>";
  */
  if(connection.getCurrentNetwork() == NETWORK_ACCESS_POINT)
    stream << "<p>WiFi Access Point</p>";
  stream << "<p>SSID: " << connection.getSSID() << "</p>"
	 << "<p>RSSI: " << connection.getRSSI() << "dBm</p>"
	 << "<p>Local IP: " << connection.getLocalIPAddress() << "</p>"
	 << "<p>Gateway: " << connection.getDefaultGateway() << "</p>"
	 << "<p>Subnet Mask: " << connection.getSubnetMask() << "</p>"
    /*  stream << "<p>SSID: " << WiFi.SSID() << "</p>"
	 << "<p>Gateway: " << WiFi.gatewayIP() << "</p>"
	 << "<p>RSSI: " << WiFi.RSSI() << "</p>"
	 << "<p>Local IP: " << WiFi.localIP() << "</p>"
    */
	 << "<p>Local Port: " << settings.localPort << "</p>"
	 << "<p>Remote IP: ";
  if(settings.autoremote)
    stream << "auto</p>";
  else if(settings.broadcast)
    stream << "broadcast</p>";
  else 
    stream << settings.remoteIPAddress << "</p>";
  stream << "<p>Remote Port: " << settings.remotePort << "</p>"
	 << "<p>MAC Address: ";
  connection.printMacAddress(stream);
  stream << "</p><p>ID: " << Particle.deviceID() << "</p>";
  /*
  byte mac[6];
  WiFi.macAddress(mac);
  for(int i=0; i<6; i++){
    if(i)
      stream.write(':');
    stream.print(mac[i], HEX);
  }
  */
  stream << "<br><button onclick='location.href=\"/reset2\"'>Clear WiFi credentials</button>";
  stream << "<br><button onclick='location.href=\"/reset99\"'>Factory Reset</button>";
  if(connection.getCurrentNetwork() != NETWORK_LOCAL_WIFI)
    stream << "<br><button onclick='location.href=\"/reconnect_sta\"'>Reconnect as WiFi Client</button>";
  if(connection.getCurrentNetwork() != NETWORK_ACCESS_POINT)
    stream << "<br><button onclick='location.href=\"/reconnect_ap\"'>Reconnect as Access Point</button>";
  stream << OSM_BACK << OSM_END;	 
  return 0;
}

int32_t process_settings(const char* u, wiced_http_response_stream_t* s, void* arg, 
			 wiced_http_message_body_t* body){
  UrlScanner url(u);
  bool updated = false;
  const char* param = url.getParameter("localport");
  if(param != NULL){
    settings.localPort = atol(param);
    updated = true;
  }
  param = url.getParameter("remoteport");
  if(param != NULL){
    settings.remotePort = atol(param);
    updated = true;
  }
  param = url.getParameter("remoteip");
  if(param != NULL){
    setRemoteIpAddress(param);
    updated = true;
  }
  Streamer stream(s);
  stream << OSM_BEGIN << "<h1>Network Settings</h1>"
	 << "<form action='/settings' method='GET'>"
	 << "<p>Local IP</p><p>" << connection.getLocalIPAddress() << "</p>"
	 << "<p>Local Port</p><input type='text' name='localport' value='"
	 << settings.localPort << "'><br>"	 
    	 << "<p>Remote IP</p><input type='text' name='remoteip' value='";
  if(settings.autoremote)
    stream << "auto'><br>";
  else if(settings.broadcast)
    stream << "broadcast'><br>";
  else 
    stream << settings.remoteIPAddress << "'><br>";
  stream << "<p>Remote Port</p><input type='text' name='remoteport' value='"
	 << settings.remotePort << "'><br>"
	 << "<button type='submit'>Update</button></form>";
  if(updated){
    stream << "<h3>Settings updated</h3>";
    reload();
  }
  if(settings.hasChanged())
    stream << "<br><button onclick='location.href=\"/save\"'>Store Settings</button>";
  stream << "<br><button onclick='location.href=\"/reset0\"'>Reset Settings</button>";
  stream << OSM_BACK << OSM_END;
  return 0;
}

int32_t process_ranges(const char* u, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  UrlScanner url(u);
  const char mins[] = "0123";
  const char maxs[] = "4567";
  bool updated = false;
  for(int i=0; i<4; ++i){
    char* param = url.getParameter(&mins[i], 1);
    if(param != NULL){
      debug << "setting input min [" << i << "] to value [" << param << "]\r\n";
      settings.min[i] = atof(param);
      updated = true;
    } 
    param = url.getParameter(&maxs[i], 1);
    if(param != NULL){
      debug << "setting input max [" << i << "] to value [" << param << "]\r\n";
      settings.max[i] = atof(param);
      updated = true;
    }
  }
  Streamer stream(s);
  stream << OSM_BEGIN << "<h1>Value Ranges</h1><form action='/ranges' method='GET'>";
  stream << "<h2>Input</h2>"
	 << "<p>CV A Min</p><input type='text' name='0' value='" << settings.min[CV_A_IN] << "'><br>"
	 << "<p>CV A Max</p><input type='text' name='4' value='" << settings.max[CV_A_IN] << "'><br>"
	 << "<p>CV B Min</p><input type='text' name='1' value='" << settings.min[CV_B_IN] << "'><br>"
	 << "<p>CV B Max</p><input type='text' name='5' value='" << settings.max[CV_B_IN] << "'><br>"
	 << "<h2>Output</h2>"
	 << "<p>CV A Min</p><input type='text' name='2' value='" << settings.min[CV_A_OUT] << "'><br>"
	 << "<p>CV A Max</p><input type='text' name='6' value='" << settings.max[CV_A_OUT] << "'><br>"
	 << "<p>CV B Min</p><input type='text' name='3' value='" << settings.min[CV_B_OUT] << "'><br>"
	 << "<p>CV B Max</p><input type='text' name='7' value='" << settings.max[CV_B_OUT] << "'><br>"
	 << "<button type='submit'>Update</button></form>";
  if(updated){
    stream << "<h3>Settings updated</h3>";
    reload();
  }
  if(settings.hasChanged())
    stream << "<br><button onclick='location.href=\"/save\"'>Store Settings</button>";
  stream << "<br><button onclick='location.href=\"/reset0\"'>Reset Settings</button>";
  stream << OSM_BACK << OSM_END;
  return 0;
}

int32_t process_address(const char* u, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  UrlScanner url(u);
  const char inputs[] = "01234";
  const char outputs[] = "56789";
  bool updated = false;
  for(int i=0; i<5; ++i){
    char* param = url.getParameter(&inputs[i], 1);
    if(param != NULL){
      debug << "setting input address [" << i << "] to value [" << param << "]\r\n";
      settings.setInputAddress(i, param);
      updated = true;
    }
    param = url.getParameter(&outputs[i], 1);
    if(param != NULL){
      debug << "setting output address [" << i << "] to value [" << param << "]\r\n";
      settings.setOutputAddress(i, param);
      updated = true;
    }
  }
  char* param = url.getParameter("ip", 2);
  if(param != NULL){
    debug << "setting input prefix [" << param << "]\r\n";
    settings.setInputPrefix(param);
    updated = true;
  }
  param = url.getParameter("op", 2);
  if(param != NULL){
    debug << "setting output prefix [" << param << "]\r\n";
    settings.setOutputPrefix(param);
    updated = true;
  }
  Streamer stream(s);
  stream << OSM_BEGIN << "<h1>Address Mapping</h1><form action='/address' method='GET'>";
  stream << "<h2>Receive</h2>"
	 << "<p>Prefix</p><input type='text' name='ip' value='" << settings.inputPrefix << "'><br>"
	 << "<p>Status</p><input type='text' name='0' value='" << settings.getInputAddress(0) << "'><br>"
	 << "<p>CV A</p><input type='text' name='1' value='" << settings.getInputAddress(1) << "'><br>"
	 << "<p>CV B</p><input type='text' name='2' value='" << settings.getInputAddress(2) << "'><br>"
	 << "<p>Trigger A</p><input type='text' name='3' value='" << settings.getInputAddress(3) << "'><br>"
	 << "<p>Trigger B</p><input type='text' name='4' value='" << settings.getInputAddress(4) << "'><br>";
  stream << "<h2>Send</h2>"
	 << "<p>Prefix</p><input type='text' name='op' value='" << settings.outputPrefix << "'><br>"
	 << "<p>Status</p><input type='text' name='5' value='" << settings.getOutputAddress(0) << "'><br>"
	 << "<p>CV A</p><input type='text' name='6' value='" << settings.getOutputAddress(1) << "'><br>"
	 << "<p>CV B</p><input type='text' name='7' value='" << settings.getOutputAddress(2) << "'><br>"
	 << "<p>Trigger A</p><input type='text' name='8' value='" << settings.getOutputAddress(3) << "'><br>"
	 << "<p>Trigger B</p><input type='text' name='9' value='" << settings.getOutputAddress(4) << "'><br>"
	 << "<button type='submit'>Update</button></form>";
  if(updated){
    stream << "<h3>Settings updated</h3>";
    reload();
  }
  if(settings.hasChanged())
    stream << "<br><button onclick='location.href=\"/save\"'>Store Settings</button>";
  stream << "<br><button onclick='location.href=\"/reset0\"'>Reset Settings</button>";
  stream << OSM_BACK << OSM_END;
  return 0;
}

int32_t process_auth(const char* url, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  //  debug << "msg body [" << body->message_data_length << "/" << body->total_message_data_remaining << "]\r\n";
  //  debug << "msg url [" << url << "]\r\n";
  Streamer stream(s);
  UrlScanner params(body->data, body->message_data_length);
  char* ssid = params.getParameter("ssid");
  char* pass = params.getParameter("password");
  char* auth = params.getParameter("auth");
  stream << OSM_BEGIN << "<h1>WiFi Credentials</h1>";
  if(ssid != NULL && pass != NULL && auth != NULL){
    auth[1] = '\0';
    connection.setCredentials(ssid, pass, auth);
    stream << "<br><button onclick='location.href=\"/reconnect_sta\"'>Reconnect as WiFi Client</button>";
  }else{
    stream << "<form action='auth' method='POST'>"
	   << "<p>SSID</p><input name='ssid' type='text'><br>"
	   << "<p>Password</p><input name='password' type='password'><br>"
	   << "<p>Authentication</p><select name='auth'>"
	   << "<option value='0'>Open</option>"
	   << "<option value='1'>WEP</option>"
	   << "<option value='2'>WPA</option>"
	   << "<option value='3'>WPA2</option>"
	   << "</select><br>"
	   << "<button type='submit'>Connect</button></form>";
  }
  stream << OSM_BACK << OSM_END;
  //  debug << "msg body [" << body->message_data_length << "/" << body->total_message_data_remaining << "]\r\n";
  return 0;
}

int32_t process_reconnect(const char* url, wiced_http_response_stream_t* s, void* a, wiced_http_message_body_t* body){
  // raw
  Streamer stream(s);
  int arg = (int)a;
  stream << OSM_BEGIN;
  if(arg == 1){
    stream.write("<h2>Reconnecting as WiFi Access Point</h2>");
    connection.connect(NETWORK_ACCESS_POINT); // asynchronous
  }else if(arg == 2){
    stream.write("<h2>Reconnecting as WiFi Client</h2>");
    connection.connect(NETWORK_LOCAL_WIFI); // asynchronous
  }
  stream << OSM_END;
  return 0;
}

int32_t process_save(const char* url, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  settings.saveToFlash();
  Streamer stream(s);
  stream << OSM_BEGIN;
  stream << "<h1>Saved Settings</h1><p>Device settings saved to flash</p>";
  stream << OSM_BACK << OSM_END;
  return 0;
}

int32_t process_reset(const char* u, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  Streamer stream(s);
  stream << OSM_BEGIN << "<h1>Reset Settings</h1>";	 
  UrlScanner url(u);
  int idx = (int)arg;
  const char* param = url.getParameter("confirm");
  if(param != NULL){
    switch(idx){
    case 0:
      settings.reset();
      stream << "<p>Settings reset to factory defaults</p>";
      reload();
      break;
    case 2:
      connection.clearCredentials();
      stream << "<p>WiFi credentials purged</p>";
      break;
    case 99:
      factoryReset();
      stream << "<p>Factory Reset</p>";
      break;
    }
  }else{
    stream << "<p>Are you sure?</p>"
	   << "<button onclick='location.href=\"/reset" << idx 
	   << "?confirm=yes\"'>Yes</button>";
  }
  stream << OSM_BACK << OSM_END;
  return 0;
}

#if 0
#include "Scanner.hpp"
class HtmlScanner : public Scanner {
public:
  void scan(char* ssid, int sec, int channel, int dBm, uint32_t maxRate){
    if(out != NULL){
      (*out) << "<input type='radio' name='ssid' value='"<< ssid << "'>" 
	     << ssid << ' ' << dBm << "dBm " << getSecurityName(sec) << "<br>";	
    }
  }
};

int32_t process_scan(const char* url, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  Streamer stream(s);
  HtmlScanner scanner;
  stream << OSM_BEGIN << "<h1>Scan Networks</h1>"
    	 << "<form action='auth' method='POST'>";
  debugMessage("starting scan");
  scanner.start(&stream);
  debugMessage("scanning");
  long now = millis();
  while(scanner.scanning() && (millis() - now) < 6000); // wait max 6 seconds
  scanner.done();
  debugMessage("scanning complete");
  stream << "<p>Password</p><input name='password' type='password'><br>"
	 << "<p>Authentication</p><select name='auth'>"
	 << "<option value='0'>Open</option>"
	 << "<option value='1'>WEP</option>"
	 << "<option value='2'>WPA</option>"
	 << "<option value='3' selected>WPA2</option>"
	 << "</select><br>"
	 << "<button type='submit'>Connect</button></form>"
	 << OSM_BACK << OSM_END;
  return 0;
}
#endif
