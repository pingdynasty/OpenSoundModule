
#define MESSAGE_QUEUE_MAX_MESSAGES 16

class OscMessage {
public:
  char* address;
  char* types;
  uint8_t* data;
  uint8_t* end;
public:
  OscMessage() : address(NULL), types(NULL), data(NULL), end(NULL){}

  OscMessage(uint8_t* data, int size) {
    setBuffer(data, size);
    // parse();
  }

  void clear(){
    types = address;
    data = address;
    end = address;
  }

  int getAddressLength(){
    // includes 0 padding
    return types-address;
  }

  int calculateAddressLength(){
    return strnlen(address, end-address)+1;
  }

  int getTypeTagLength(){
    return data-types;
  }

  int calculateTypeTagLength(){
    return strnlen(types, end-types)+1;
  }

  int getPrefixLength(){
    return data-address;
  }

  int getTotalDataLength(){
    // todo: 'end' might specify capacity, not end position
    return end-data;
  }

  int getMessageLength(){
    // todo: 'end' might specify capacity, not end position
    return end-address;
  }

  int calculateMessageLength(){
    int size = getPrefixLength();
    for(int i=0; i<getSize(); ++i)
      size += getDataSize(i);
    return size;
    // return size+calculateDataLength();
  }

  void setBuffer(uint8_t* buffer, int length){
    // In a stream-based protocol such as TCP, the stream should begin with an int32 giving the size of the first packet, followed by the contents of the first packet, followed by the size of the second packet, etc.
    address = (char*)buffer;
    end = buffer+length;
  }

  // assumes address and end have been set
  void parse(){
    // Note: some older implementations of OSC may omit the OSC Type Tag string. Until all such implementations are updated, OSC implementations should be robust in the case of a missing OSC Type Tag String.
    types = address+calculateAddressLength();
    // while(*types == '\0' && ++types < end);
    while((types & 0x03) && ++types < end);
    data = (uint8_t*)types+calculateTypeTagLength();
    // while(*data == '/0') && (data & 0x03) && ++data < end);
    while((data & 0x03) && ++data < end);
  }

  char* getAddress(){
    return address;
  }

  bool matches(const char* pattern){
    return matches(pattern, strlen(pattern));
  }

  bool matches(const char* pattern, int length){
    // return strncmp(address, pattern, length) == 0;
    // todo: wild cards?
    return strncasecmp(address, pattern, length) == 0;
  }
 
  /*
   * @return number of arguments (data fields) in the message
   */
  uint8_t getSize(){
    uint8_t size = 0;
    for(int i=1; types[i] != '\0' && &types[i] < data; ++i)
      size++;
    return size;
  }

  char getDataType(uint8_t index){
    return types[index+1]; // the first character is a comma
  }

  int getDataSize(uint8_t index){
    char type = getDataType(index);
    switch(type){
    case 'c': // ASCII character sent as 32 bits
    case 'r': // 32bit RGBA colour
    case 'i': // 32bit integer
    case 'f': // 32bit float
    case 'm': // 4-byte MIDI message
      return 4;
    case 'h': // 64bit integer
    case 'd': // 64bit double
      return 8;
    case 'b': // blob
      // first four bytes is the size
      return *(uint32_t*)getData(index);
    case 's': // string
    case 'S': // symbol
      {
	char* str = (char*)getData(index);
	int8_t size = strnlen(str, end-str)+1;
	while(size & 0x03)
	  size++;
	return size;
      }
    default:
      return 0;
    }
  }

  float getAsFloat(uint8_t i){
    float f;
    switch(getDataType(i)){
    case 'f':
      f = getFloat(i);
      break;
    case 'i':
      f = float(getInt(i));
      break;
    case 'd':
      f = float(getDouble(i));
      break;
    case 'h':
      f = float(getLong(i));
      break;
    case 'T':
      f = 1.0f;
      break;
    default:
      f = 0.0f;
      break;
    }
    return f;
  }

  bool getAsBool(uint8_t i){
    bool b;
    switch(getDataType(i)){
    case 'f':
      b = getFloat(i) > 0.5;
      break;
    case 'i':
      b = getInt(i) != 0;
      break;
    case 'd':
      b = getDouble(i) > 0.5;
      break;
    case 'h':
      b = getLong(i) != 0;
      break;
    case 'T':
      b = true;
      break;
    default:
      b = false;
      break;
    }
    return b;
  }

  void* getData(uint8_t index){
    uint8_t* ptr = data;
    for(int i=0; i<index; ++i)
      ptr += getDataSize(i);
    return ptr;
  }

  int32_t getInt(int8_t index){
    return getOscInt32((uint8_t*)getData(index));
  }

  float getFloat(int8_t index){
    getOscFloat32((uint8_t*)getData(index));
  }

  int64_t getLong(int8_t index){
    return getOscInt64((uint8_t*)getData(index));
  }

  double getDouble(int8_t index){
    getOscFloat64((uint8_t*)getData(index));
  }

  char* getString(int8_t index){
    int offset = getOffset(index);
    return (char*)(data+offset);
  }

  static int32_t getOscInt32(uint8_t* data){
    union { int32_t i; uint8_t b[4]; } u;
    u.b[3] = data[0];
    u.b[2] = data[1];
    u.b[1] = data[2];
    u.b[0] = data[3];
    return u.i;
  }

  static float getOscFloat32(uint8_t* data){
    union { float f; uint8_t b[4]; } u;
    u.b[3] = data[0];
    u.b[2] = data[1];
    u.b[1] = data[2];
    u.b[0] = data[3];
    return u.f;
  }

  static int64_t getOscInt64(uint8_t* data){
    union { int64_t i; uint8_t b[8]; } u;
    u.b[7] = data[0];
    u.b[6] = data[1];
    u.b[5] = data[2];
    u.b[4] = data[3];
    u.b[3] = data[4];
    u.b[2] = data[5];
    u.b[1] = data[6];
    u.b[0] = data[7];
    return u.i;
  }

  static float getOscFloat64(uint8_t* data){
    union { double d; uint8_t b[8]; } u;
    u.b[7] = data[0];
    u.b[6] = data[1];
    u.b[5] = data[2];
    u.b[4] = data[3];
    u.b[3] = data[4];
    u.b[2] = data[5];
    u.b[1] = data[6];
    u.b[0] = data[7];
    return u.d;
  }

  void setPrefix(const char* addr, const char* tags){    
    // strncpy(address, addr, end-address);
    // If the length of src is less than n, strncpy() writes additional null
    // bytes to dest to ensure that a total of n bytes are written.
    strcpy(address, addr);
    types = address+calculateAddressLength();
    while((types & 0x03) && types+1 < end)
      *types++ = '\0'; // fill end of address with 0 padding
    // strncpy(types, tags, end-types);
    strcpy(types, tags);
    data = (uint8_t*)types+calculateTypeTagLength();
    while((data & 0x03) && data+1 < end)
      *data++ = '\0';
  }

  void write(Print& out){
    out.write(address, calculateMessageLength());
  }

  void setData(int index, void* data, int size){
    void* ptr = getData(index);
    memcpy(ptr, data, size);
  }

  void setFloat(uint8_t index, float value){
    // setData(index, &value, 4);
    set32(index, (uint8_t*)&value);
  }

  void setInt(uint8_t index, int32_t value){
    set32(index, (uint8_t*)&value);
  }

  void setDouble(uint8_t index, double value){
    set64(index, (uint8_t*)&value);
  }

  void setLong(uint8_t index, int64_t value){
    set64(index, (uint8_t*)&value);
  }

  void setString(uint8_t index, const char* str){
    setString(index, str, strlen(str));
  }

  void setString(uint8_t index, const char* str, size_t length){
    char* ptr = (char*)getData(index);
    for(size_t i=0; i<length && ptr < end-1; ++i)
      *ptr++ = str[i];
    do{
      *ptr++ = '\0';
    }while(ptr & 0x03); // pad to 4 bytes
  }

protected:
  void set32(uint8_t index, uint8_t* value){
    uint8_t* ptr = (uint8_t*)getData(index);
    *ptr++ = value[3];
    *ptr++ = value[2];
    *ptr++ = value[1];
    *ptr++ = value[0];
  }

  void set64(uint8_t index, uint8_t* value){
    uint8_t* ptr = (uint8_t*)getData(index);
    *ptr++ = value[7];
    *ptr++ = value[6];
    *ptr++ = value[5];
    *ptr++ = value[4];
    *ptr++ = value[3];
    *ptr++ = value[2];
    *ptr++ = value[1];
    *ptr++ = value[0];
  }
};

// #include "OscMessage.hpp"

class CircularBuffer;

class MessageQueue {
  OscMessage messages[MESSAGE_QUEUE_MAX_MESSAGES]; // an OscMessage is 16 bytes
  volatile uint8_t head;
  volatile uint8_t tail;
private:
public:
  OscMessage* popNextMessage(){
    if(tail == head)
      return NULL;
    OscMessage* msg = messages[tail];
    if(++tail >= MESSAGE_QUEUE_MAX_MESSAGES)
      tail = 0;
    return msg;
  }
// note:
// if circular buffer is used for msg data, a msg must not cross buffer boundary
  OscMessage* pushNextMessage(uint8_t* msg, int size){
    if(++head >= MESSAGE_QUEUE_MAX_MESSAGES)
      head = 0;
    if(head == tail){
      // risk of clobbering a message while it is processed: reverse instead
      // todo: signal a buffer overflow
      if(head-- >= MESSAGE_QUEUE_MAX_MESSAGES)
	head = MESSAGE_QUEUE_MAX_MESSAGES-1;
      return;
    }
    messages[head].setBuffer(msg, size);
  }
  
};
