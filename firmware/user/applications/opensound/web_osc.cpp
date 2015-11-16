#include "web.hpp"
#include "opensound.h"
#include "application.h"
#include "ApplicationSettings.h"
    
int32_t process_cvout(const char* u, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  UrlScanner url(u);
  const char* param = url.getParameter("v");
  if(param != NULL){
    uint16_t v = atol(param);
    if((int)arg == 0)
      setCVA(v);
    else if((int)arg == 1)
      setCVB(v);
  }
  return 0;
}

int32_t process_trout(const char* url, wiced_http_response_stream_t* s, void* arg, wiced_http_message_body_t* body){
  if((int)arg == 0)
    toggleTriggerA();
  else if((int)arg == 1)
    toggleTriggerB();
  return 0;
}
