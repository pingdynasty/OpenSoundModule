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
    debugMessage("ws handshake req");
    /* - MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: server has received the WebSocket
     *   handshake request. `ev_data` contains parsed HTTP request. */
    struct http_message* hm = (struct http_message*)ev_data;
    debug << "ws uri [" << hm->uri.p << "][" << hm->uri.len << "]\r\n";
    if(websocketserver.processHandshake(hm->uri.p) != 0){
      static const char notfound[] = "HTTP/1.1 404 Not Found\r\n\r\n";
      mg_send_websocket_frame(nc, WEBSOCKET_OP_CLOSE, notfound, sizeof(notfound));
    }
    break;
  }
  case MG_EV_WEBSOCKET_HANDSHAKE_DONE: 
    debugMessage("ws handshake done");
    /* - MG_EV_WEBSOCKET_HANDSHAKE_DONE: server has completed the WebSocket
     *   handshake. `ev_data` is `NULL`. */
    break;
  case MG_EV_WEBSOCKET_FRAME: {
    debugMessage("ws frame");
    /* - MG_EV_WEBSOCKET_FRAME: new WebSocket frame has arrived. `ev_data` is
     *   `struct websocket_message *` */
    struct websocket_message *wm = (struct websocket_message*)ev_data;
    debug << "frame [" << wm->flags << "][" << wm->size << "]\r\n";
    /* New websocket message. */
    if(wm->flags & WEBSOCKET_OP_TEXT)
      websocketserver.processTextFrame(wm->data, wm->size);
    else if(wm->flags & WEBSOCKET_OP_BINARY)
      websocketserver.processBinaryFrame(wm->data, wm->size);
    break;
  }
  case MG_EV_CLOSE: {
    debugMessage("ws close");
    /* Disconnect. */
    if(is_websocket(nc)){
      // do we send a close frame?
    }
    break;
  }
  }
}

WebSocketServer::WebSocketServer(const unsigned port)
{ // : TCPServer(port) {
    debugMessage("ws ctor");
}

int WebSocketServer::processHandshake(const char* uri){
  if(strncmp(uri, "/osc", 4) == 0){
    service = OSC_SERVICE;
    return 0;
  }
  if(strncmp(uri, "/midi", 5) == 0){
    service = MIDI_SERVICE;
    return 0;
  }
  if(strncmp(uri, "/echo", 5) == 0){
    service = ECHO_SERVICE;
    return 0;
  }
  if(strncmp(uri, "/status", 7) == 0){
    service = STATUS_SERVICE;
    return 0;
  }
  service = NO_SERVICE;
  return -1;
}

void WebSocketServer::begin(){
  debugMessage("ws begin");
  mg_mgr_init(&mgr, NULL);
  struct mg_connection *nc;
  nc = mg_bind(&mgr, s_http_port, ev_handler); // does this block?
  // s_http_server_opts.document_root = ".";
  mg_set_protocol_http_websocket(nc);
}

void WebSocketServer::stop(){
  debugMessage("ws stop");
  mg_mgr_free(&mgr);
}

void WebSocketServer::loop(){
  mg_mgr_poll(&mgr, 20);
}

void WebSocketServer::sendBinaryFrame(uint8_t* data, size_t dataSize){
  debug << "ws tx binary [" << dataSize << "]\r\n";
  for(struct mg_connection *c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c))
    mg_send_websocket_frame(c, WEBSOCKET_OP_BINARY, data, dataSize);
}

void WebSocketServer::sendTextFrame(uint8_t* data, size_t dataSize){
  debug << "ws tx text [" << data << "][" << dataSize << "]\r\n";
  for(struct mg_connection *c = mg_next(&mgr, NULL); c != NULL; c = mg_next(&mgr, c))
    mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, data, dataSize);
// void mg_send_websocket_frame(struct mg_connection *nc, int op_and_flags,
//                              const void *data, size_t data_len);
}
