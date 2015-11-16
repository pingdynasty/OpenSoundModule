#include "http_server.h"
#include "application.h"
#include "opensound.h"
#include "network_interface.h"

// extern "C" wiced_http_page_t osm_http_pages[];

extern "C" {
#include "web.h"
};

class WebServer {
private:
  wiced_http_server_t server;
  bool running;
public:
  WebServer() : running(false) {}
  bool begin(){
    // server, port, max sockets, pages, interface, url processor stack size
    //    wiced_http_server_start(&server, 80, 4, osm_http_pages, WICED_AP_INTERFACE, 1024);
    wiced_result_t result = wiced_http_server_start(&server, 80, 4, osm_http_pages, current_network_if, 1024);
    running =  result == WICED_SUCCESS;
    return running;
  }
  void stop(){
    if(running)
      wiced_http_server_stop(&server);
    running = false;
  }
};

extern WebServer webserver;
void configureWeb();

class UrlScanner {
private:
  const char* url;
  int len;
public:
  UrlScanner(const char* u) : url(u) {
    len = strlen(url);
    //    debug << "url: " << url << "[" << len << "]\r\n";
  }
  UrlScanner(const void* u, size_t l) : url((const char*)u), len(l)  {
    //    debug << "body: " << url << "[" << len << "]\r\n";
  }
  char* getParameter(const char* name){
    return getParameter(name, strlen(name));
  }
  char* getParameter(const char* name, size_t nlen){
    char* found = NULL;
    int i = 0;
    while(i<len && found == NULL){
      //      debug << "looking for [" << name << "][" << nlen << "] in [" << &url[i] << "][" << len << "]\r\n";
      if(strncmp(&url[i], name, nlen) == 0){
	i += nlen+1;
	found = (char*)&url[i];
      }
      do{
	i++; // fast forward to next parameter or end of list
      }while(i < len && url[i] != '&' && url[i] != '\x00' && url[i] != '\n');
      if(i < len){
	if(url[i] == '&'){
	  // replace & with \0 at the end of the parameter value
	  char* tmp = (char*)url;
	  tmp[i] = '\x00';
	}
	i++;
      }
    }
    return found;
  }
};

class Streamer : public Print {
private:
  wiced_http_response_stream_t* stream;
public:
  Streamer(wiced_http_response_stream_t* s) : stream(s){}
  // void write(const char* data){
  //   wiced_http_response_stream_write(stream, data, strlen(data));
  // }
  size_t write(const char* data){
    return write(data, strlen(data));
  }
  size_t write(uint8_t data){
    return write(&data, 1);
  }
  size_t write(const void* data, size_t size){
    wiced_http_response_stream_write(stream, data, size);
    return size;
  }
  void flush(){
    wiced_http_response_stream_flush(stream);
  }
};

#if 0 // superceded by Print template in opensound.h
inline Streamer &operator <<(Streamer &obj, const char* arg) { 
  obj.write(arg); 
  return obj; 
}
#endif
