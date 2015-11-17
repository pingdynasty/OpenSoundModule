#include "wiced.h"

class Scanner {
protected:
  volatile bool processing = false;
  Print* out;
public:
  Scanner() : out(NULL) {}

  bool start(Print* destination){
    out = destination;
    processing = true;
    wiced_result_t ret = wiced_wifi_scan_networks(scan_handler, this);
    return ret == WICED_SUCCESS;
  }

  bool scanning(){
    return processing;
  }

  virtual void done(){
    processing = false;
    if(out)
      out->println("scan complete");
    out = NULL;
  }

  virtual void scan(char* ssid, int sec, int channel, int dBm, uint32_t maxRate){
    if(out){
      out->print("scan SSID: ");
      out->print((char*)ssid);
      out->print("\tsecurity: ");
      out->print(sec);
      out->print("\tchannel: ");
      out->print(channel);
      out->print("\tmax rate: ");
      out->print(maxRate);
      out->print("\tRSSI: ");
      out->print(dBm);
      out->println("dBm");
    }
  }

  static const char* getSecurityName(int sec){
    if(sec == 0)
      return "Open";
    if(sec & WPA2_SECURITY)
      return "WPA2";
    if(sec & WPA_SECURITY)
      return "WPA";
    if(sec & WEP_ENABLED)
      return "WEP";
    return "Unknown";
    /*
    switch(sec){
    case WICED_SECURITY_OPEN:
      return "Open";
    case WICED_SECURITY_WEP_PSK:
      return "WEP";
    case WICED_SECURITY_WPA_TKIP_PSK:
      return "WPA";
    case WICED_SECURITY_WPA2_AES_PSK:
      return "WPA2";
    */
  }

  static wiced_result_t scan_handler(wiced_scan_handler_result_t* malloced_scan_result){
    Scanner* scanner = (Scanner*)malloced_scan_result->user_data;
    malloc_transfer_to_curr_thread( malloced_scan_result );
    if(malloced_scan_result->status != WICED_SCAN_ABORTED){
      wiced_scan_result_t& ap_details = malloced_scan_result->ap_details;
      // unsigned ssid_len = ap_details.SSID.length > 32 ? 32 : ap_details.SSID.length;
      scanner->scan((char*)ap_details.SSID.value, ap_details.security, ap_details.channel,
		    ap_details.signal_strength, ap_details.max_data_rate);
    // WICED_SCAN_INCOMPLETE,
    // WICED_SCAN_COMPLETED_SUCCESSFULLY,
    // WICED_SCAN_ABORTED,
      /*
      memcpy(entry.ssid, ap_details.SSID.value, ssid_len);
      entry.ssid[ssid_len] = 0;
      entry.rssi = ap_details.signal_strength;
      entry.security = ap_details.security;
      entry.channel = ap_details.channel;
      entry.max_data_rate = ap_details.max_data_rate;
    }
  else
    {
      entry.done = 1;
      */
    }
    if(malloced_scan_result->status != WICED_SCAN_INCOMPLETE)
      scanner->done();
    free(malloced_scan_result);
    return WICED_SUCCESS;
  }
};
