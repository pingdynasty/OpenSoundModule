#include "WebSocketServer.h"
#include "mongoose.h"

static struct mg_mgr mgr;
static const char *s_http_port = "8008";
// static struct mg_serve_http_opts s_http_server_opts;

extern WebSocketServer websocketserver;

extern "C"{
  int gettimeofday(struct timeval *t, void *tz){
    return 0;
  }
}

static int is_websocket(const struct mg_connection *nc) {
  return nc->flags & MG_F_IS_WEBSOCKET;
}

static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
  switch (ev) {
  case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: {
    /* - MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: server has received the WebSocket
     *   handshake request. `ev_data` contains parsed HTTP request. */
    struct http_message* hm = (struct http_message*)ev_data;
    if(websocketserver.processHandshake(hm->uri.p, hm->uri.len) != 0){
      static const char notfound[] = "HTTP/1.1 404 Not Found\r\n\r\n";
      mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, notfound, sizeof(notfound));
    }
    break;
  }
#ifdef SERIAL_DEBUG
  case MG_EV_WEBSOCKET_HANDSHAKE_DONE: 
    /* - MG_EV_WEBSOCKET_HANDSHAKE_DONE: server has completed the WebSocket
     *   handshake. `ev_data` is `NULL`. */
    debugMessage("ws handshake done");
    break;
#endif
  case MG_EV_WEBSOCKET_FRAME: {
    /* - MG_EV_WEBSOCKET_FRAME: new WebSocket frame has arrived. `ev_data` is
     *   `struct websocket_message *` */
    struct websocket_message *wm = (struct websocket_message*)ev_data;
#ifdef SERIAL_DEBUG
    debug << "frame [" << wm->flags << "][" << wm->size << "]\r\n";
#endif
    /* New websocket message. */
    if(wm->flags & WEBSOCKET_OP_TEXT)
      websocketserver.processTextFrame((char*)(wm->data), wm->size);
    else if(wm->flags & WEBSOCKET_OP_BINARY)
      websocketserver.processBinaryFrame(wm->data, wm->size);
    break;
  }
  case MG_EV_CLOSE: {
    /* Disconnect. */
#ifdef SERIAL_DEBUG
    debugMessage("ws close");
#endif
    if(is_websocket(nc)){
      // do we send a close frame?
    }
    break;
  }
  }
}

WebSocketServer::WebSocketServer(const unsigned port){
}

int WebSocketServer::processHandshake(const char* uri, size_t len){
  // todo: support multiple connections with different services
  service = NO_SERVICE;
  if(strncmp(uri, "/osc", len) == 0)
    service = OSC_SERVICE;
  else if(strncmp(uri, "/midi", len) == 0)
    service = MIDI_SERVICE;
  else if(strncmp(uri, "/echo", len) == 0)
    service = ECHO_SERVICE;
  else if(strncmp(uri, "/status", len) == 0)
    service = STATUS_SERVICE;
#ifdef SERIAL_DEBUG
  debug << "ws handshake [" << uri << "][" << len << "][" << service << "]\r\n";
#endif
  return service == NO_SERVICE ? -1 : 0;
}

void WebSocketServer::begin(){
#ifdef SERIAL_DEBUG
  debugMessage("ws begin");
#endif
  mg_mgr_init(&mgr, NULL);
  struct mg_connection *nc;
  nc = mg_bind(&mgr, s_http_port, ev_handler); // does this block?
  // s_http_server_opts.document_root = ".";
  mg_set_protocol_http_websocket(nc);
}

void WebSocketServer::stop(){
#ifdef SERIAL_DEBUG
  debugMessage("ws stop");
#endif
  mg_mgr_free(&mgr);
}

void WebSocketServer::loop(){
  mg_mgr_poll(&mgr, 20);
}

void WebSocketServer::sendBinaryFrame(uint8_t* data, size_t dataSize){
#ifdef SERIAL_DEBUG
  debug << "ws tx binary [" << dataSize << "]\r\n";
#endif
  for(struct mg_connection *c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c))
    mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, data, dataSize);
}

void WebSocketServer::sendTextFrame(const char* data, size_t dataSize){
#ifdef SERIAL_DEBUG
  debug << "ws tx text [" << data << "][" << dataSize << "]\r\n";
#endif
  for(struct mg_connection *c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c))
    mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, (uint8_t*)data, dataSize);
}
