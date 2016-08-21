#ifndef WEBSOCKETSERVER_H_
#define WEBSOCKETSERVER_H_

#include "opensound.h"
// #include "websocket.h"
  
class WebSocketServer { // : public TCPServer {
private:
  enum WebSocketService {
    NO_SERVICE,
    ECHO_SERVICE,
    STATUS_SERVICE,
    MIDI_SERVICE,
    OSC_SERVICE
  };
  // uint8_t buffer[WEBSOCKET_BUFFER_SIZE];
  // int readLength;
  // size_t frameSize;
  // enum wsState state;
  // enum wsFrameType frameType;
  // struct handshake hs;
  WebSocketService service;
  // TCPClient client;
public:
WebSocketServer(const unsigned port);
 // { // : TCPServer(port) {
 //    mg_mgr_init(&mgr, NULL);
 //  }

 void begin();
 void stop();

 void loop();

  void reset(){
    // memset(buffer, 0, WEBSOCKET_BUFFER_SIZE);
    // nullHandshake(&hs);
    service = NO_SERVICE;
    // readLength = 0;
    // frameSize = WEBSOCKET_BUFFER_SIZE;
    // state = WS_STATE_OPENING;
    // frameType = WS_INCOMPLETE_FRAME;
  }

  // void prepareBuffer(){
  //   frameSize = WEBSOCKET_BUFFER_SIZE; 
  //   memset(buffer, 0, WEBSOCKET_BUFFER_SIZE);
  // }

  // void initNewFrame(){
  //   frameType = WS_INCOMPLETE_FRAME; 
  //   readLength = 0; 
  //   memset(buffer, 0, WEBSOCKET_BUFFER_SIZE);
  // }

  int processHandshake(const char* uri);

  void sendOscData(uint8_t* data, size_t dataSize){
    if(service == OSC_SERVICE){
      // uint8_t output[16];
      // base64_encode(data, dataSize, output, sizeof(output), BASE64_STANDARD);
      sendBinaryFrame(data, dataSize);
    }
  }

  void processOscData(uint8_t* data, size_t dataSize){
    process_osc(data, dataSize);
  }

  void processTextFrame(uint8_t* data, size_t dataSize){
    // switch(service){
    // case ECHO_SERVICE:
    //   processEcho(data, dataSize);
    //   break;
    // case STATUS_SERVICE:
    //   processStatus(data, dataSize);
    //   break;
    // default:
    //   break;
    // }
  }    

  void processBinaryFrame(uint8_t* data, size_t dataSize){
    switch(service){
    case OSC_SERVICE:
      processOscData(data, dataSize);
      break;
    default:
      break;
    }
  }    

  void processStatus(uint8_t* data, size_t dataSize){
    debugMessage("websocket status");
    const char msg[] = "I'm fine, thanks, how are you?";
    sendTextFrame((uint8_t*)msg, sizeof(msg)-1);
  }

  void processEcho(uint8_t* data, size_t dataSize){
    uint8_t *receivedString = NULL;
    receivedString = (uint8_t *)malloc(dataSize+1);
    ASSERT(receivedString, "malloc failed");
    memcpy(receivedString, data, dataSize);
    receivedString[ dataSize ] = 0;
    sendTextFrame(receivedString, dataSize);
    free(receivedString);
  }

  void sendBinaryFrame(uint8_t* data, size_t dataSize);

  void sendTextFrame(uint8_t* data, size_t dataSize);

  int process(){
    // uint8_t* data = NULL;
    // size_t dataSize = 0;
    // // ASSERT(frameType == WS_INCOMPLETE_FRAME, "weird frame")
    // // if(frameType == WS_INCOMPLETE_FRAME) {
    //   readLength = recv_websocket_data(buffer, WEBSOCKET_BUFFER_SIZE);
    //   ASSERT(readLength <= WEBSOCKET_BUFFER_SIZE, "recv overflow");
    //   if(readLength <= 0)
    // 	return 0;

    //   if (state == WS_STATE_OPENING) {
    // 	frameType = wsParseHandshake(buffer, readLength, &hs);
    // 	service = NO_SERVICE;
    //   } else {
    // 	frameType = wsParseInputFrame(buffer, readLength, &data, &dataSize);
    //   }
    
    //   if(frameType == WS_ERROR_FRAME || (frameType == WS_INCOMPLETE_FRAME && readLength == WEBSOCKET_BUFFER_SIZE)) {
    // 	if(frameType == WS_INCOMPLETE_FRAME)
    // 	  debugMessage("websocket buffer overflow");
    // 	else
    // 	  debugMessage("websocket error frame");
    //   	if(state == WS_STATE_OPENING) {
    // 	  prepareBuffer();
    // 	  frameSize = sprintf((char *)buffer,
    // 			      PSTR("HTTP/1.1 400 Bad Request\r\n"
    // 				   "%s%s\r\n\r\n"),
    // 			      versionField,
    // 			      version);
    // 	  send_websocket_data( buffer, frameSize );
    // 	  return -1;
    // 	} else {
    // 	  prepareBuffer();
    // 	  wsMakeFrame(NULL, 0, buffer, &frameSize, WS_CLOSING_FRAME);
    // 	  send_websocket_data( buffer, frameSize );
    // 	  state = WS_STATE_CLOSING;
    // 	  service = NO_SERVICE;
    // 	  initNewFrame();
    // 	  return -1;
    // 	}
    //   }

    //   if(state == WS_STATE_OPENING) {
    // 	ASSERT(frameType == WS_OPENING_FRAME, "invalid frame type");
    // 	if (frameType == WS_OPENING_FRAME) {
    // 	  // if resource is right, generate answer handshake and send it
    // 	  if(processHandshake(hs) != 0){
    // 	    debugMessage("websocket invalid handshake");
    // 	    return -1;
    // 	  }
    // 	  prepareBuffer();
    // 	  wsGetHandshakeAnswer(&hs, buffer, &frameSize);
    // 	  freeHandshake(&hs);
    // 	  send_websocket_data( buffer, frameSize );
    // 	  state = WS_STATE_NORMAL;
    // 	  initNewFrame();
    // 	}
    //   } else {
    //     if (frameType == WS_CLOSING_FRAME) {
    //       if(state == WS_STATE_CLOSING) {
    // 	    debugMessage("websocket closing");
    // 	    return -1;
    //       } else {
    //         prepareBuffer();
    //         wsMakeFrame(NULL, 0, buffer, &frameSize, WS_CLOSING_FRAME);
    // 	    send_websocket_data( buffer, frameSize );
    // 	    return 0;
    //       }
    //     } else if(frameType == WS_TEXT_FRAME) {
    // 	  processTextFrame(data, dataSize);
    // 	  initNewFrame();
    //     } else if(frameType == WS_BINARY_FRAME) {
    // 	  processBinaryFrame(data, dataSize);
    // 	  initNewFrame();
    //     }
    //   }
    return 0;
  }
};
#endif /* WEBSOCKETSERVER_H_ */
