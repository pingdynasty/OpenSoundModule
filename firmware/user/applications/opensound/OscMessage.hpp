#ifndef __OscMessage_h__
#define __OscMessage_h__

#include <inttypes.h>
#include <string.h>
#include "application.h"
#include "opensound.h"

#if 0
// note: big endian only
#define htonl(A)    ((((uint32_t)(A) & 0xff000000) >> 24) | \
                     (((uint32_t)(A) & 0x00ff0000) >> 8) | \
                     (((uint32_t)(A) & 0x0000ff00) << 8) | \
                     (((uint32_t)(A) & 0x000000ff) << 24))

class OscBase {
private:
  uint8_t* prefix;
  uint8_t* types;
  uint8_t* data;
public:
  int getDataSize(int offset, char type){
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
    case 's': // string
    case 'S': // symbol
    case 'b': // blob
      // todo - strnlen for overflow protection
      return strlen(data+offset);
    default:
      return 0;
    }
  }

  char getDataType(int8_t index){
    return types[index];
  }

  uint8_t* getDataPointer(int8_t index){
    int offset = 0;
    for(int i=0; i<index && types[i] != '/0'; ++i)
      offset += getDataSize(offset, types[i]);
    return data+offset;
  }
};

class OscReader : public OscMessage {
public:
  void parsePacket(uint8_t* buffer, int size);
  void parseBundle(uint8_t* buffer, int size);
  void parseMessage(uint8_t* buffer, int size);
  void parsePrefix(uint8_t* buffer, int size);
  void parseAddress(uint8_t* buffer, int size);
  void parseTypes(uint8_t* buffer, int size);
  char* getAddress();
  char* getTypes();
  uint8_t* getData();
};

class OscWriter : public OscMessage {
public:
  void setAddress(char* address);
  void addType(char type);
  void setInt(int index, int data);
  void setFloat(int index, float data);
  void setString(int index, char* data);
};
#endif

/*
  todo: define get/set methods with two different indexes: field and offset
  void* getDataAt(offset) / getField(index)
*/
class OscMessage {
public:
  uint8_t prefix[OSC_MESSAGE_MAX_PREFIX_SIZE];
  uint8_t prefixLength;
  uint8_t* types;
  uint8_t data[OSC_MESSAGE_MAX_DATA_SIZE];
  uint8_t dataLength;
public:
  OscMessage() : prefixLength(0), types(NULL), dataLength(0){}

  OscMessage(const char* a) : prefixLength(0), types(NULL), dataLength(0){
    // clear();
    setAddress(a);
  }

  void clear(){
    memset(prefix, 0, sizeof(prefix));
    memset(data, 0, sizeof(data));
    prefixLength = 0;
    dataLength = 0;
  }

  void parse(uint8_t* buffer, int length){
    // clear();
    setAddress((const char*)buffer);
    int i = prefixLength;
    while(buffer[i] != '\0'){
      char type = (char)buffer[i++];
      addType(type, getDataSize(type));
    }
    prefix[i++] = '\0'; // add space for at least one \0
    while(i & 3) // pad to 4 bytes
      i++;
    memcpy(data, &buffer[i], length-i);    
  }

  int getSize(){
    int size = 0;
    while(types[size] != '\0')
      size++;
    return size;
  }

  char* getAddress(){
    return (char*)prefix;
  }

  int8_t getDataSize(char type){
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
    case 's': // string
    case 'S': // symbol
    case 'b': // blob
      // todo - but what?!
    default:
      return 0;
    }
  }

  char getDataType(int8_t index){
    return types[index];
  }

  int getOffset(int8_t index){
    int offset = 0;
    for(int i=0; i<index; ++i)
      offset += getDataSize(types[i]);
    return offset;
  }

  static int32_t getOscInt(uint8_t* data){
    int index = 0;
    union { int32_t i; uint8_t b[4]; } u;
    u.b[3] = data[index++];
    u.b[2] = data[index++];
    u.b[1] = data[index++];
    u.b[0] = data[index];
    return u.i;
  }


  int32_t getInt(int8_t index){
    index = getOffset(index);
    union { int32_t i; uint8_t b[4]; } u;
    u.b[3] = data[index++];
    u.b[2] = data[index++];
    u.b[1] = data[index++];
    u.b[0] = data[index];
    return u.i;
  }

  float getFloat(int8_t index){
    index = getOffset(index);
    union { float f; uint8_t b[4]; } u;
    u.b[3] = data[index++];
    u.b[2] = data[index++];
    u.b[1] = data[index++];
    u.b[0] = data[index];
    return u.f;
    //    return *(float*)getString(index);
    /*
    int offset = getOffset(index);
    union { float f; uint8_t b[4]; } u;
    memcpy(u.b, &data[offset], 4);    
    return htonl(u.f);
    */
  }

  double getDouble(int8_t index){
    index = getOffset(index);
    union { double d; uint8_t b[8]; } u;
    u.b[7] = data[index++];
    u.b[6] = data[index++];
    u.b[5] = data[index++];
    u.b[4] = data[index++];
    u.b[3] = data[index++];
    u.b[2] = data[index++];
    u.b[1] = data[index++];
    u.b[0] = data[index];
    return u.d;
  }

  int64_t getLong(int8_t index){
    index = getOffset(index);
    union { int64_t i; uint8_t b[8]; } u;
    u.b[7] = data[index++];
    u.b[6] = data[index++];
    u.b[5] = data[index++];
    u.b[4] = data[index++];
    u.b[3] = data[index++];
    u.b[2] = data[index++];
    u.b[1] = data[index++];
    u.b[0] = data[index];
    return u.i;
  }

  char* getString(int8_t index){
    int offset = getOffset(index);
    return (char*)(data+offset);
  }

  void setAddress(const char* a){
    prefixLength = strnlen(a, OSC_MESSAGE_MAX_PREFIX_SIZE-5)+1;
#if 1 // reset prefix and data
    types = NULL;
    dataLength = 0;
#else // copy over
    if(types != NULL){
      uint8_t pos = prefixLength;
      while(pos & 3) pos++; // pad to 4 bytes
      pos++; // add one for ','
      int sz = min(sizeof(prefix)-(types-prefix), sizeof(prefix)-pos);
      memmove(prefix+pos, types, sz-1);
    }
#endif
    //    memset(address, 0, sizeof(address));
    memcpy(prefix, a, prefixLength);
    while(prefixLength & 3) // pad to 4 bytes
      prefix[prefixLength++] = '\0';
    prefix[prefixLength++] = ',';
    types = prefix+prefixLength;
  }

  int copy(uint8_t* buf, int buflen){
    if(buflen < prefixLength)
      return -1;
    memcpy(buf, prefix, prefixLength);
    int len = prefixLength;
    buf[len++] = '\0';
    while(len & 3) // pad to 4 bytes
      buf[len++] = '\0';
    if(buflen < len+dataLength)
      return -1;
    memcpy(buf+len, data, dataLength);
    len += dataLength;
    while(len & 3) // pad to 4 bytes
      buf[len++] = '\0';
    return len;
  }

  void write(Print& out){
    out.write(prefix, prefixLength);
    // add zero padding
    switch((prefixLength) & 3){
    case 0:
      out.write((uint8_t)'\0');
    case 1:
      out.write((uint8_t)'\0');
    case 2:
      out.write((uint8_t)'\0');
    case 3:
      out.write((uint8_t)'\0');
    }
    out.write(data, dataLength);
  }

  uint8_t addFloat(float value){
    return add('f', (uint8_t*)&value);
  }
  uint8_t addInt(int32_t value){
    return add('i', (uint8_t*)&value);
  }
  void addString(){
    addType('s', 0);
  }
  uint8_t addString(const char* str){
    return addString((uint8_t*)str, strnlen(str, OSC_MESSAGE_MAX_DATA_SIZE-dataLength));
  }
  uint8_t addString(uint8_t* value, size_t sz){
    addType('s', 0);
    uint8_t index = dataLength;
    for(size_t i=0; i<sz; ++i)
      data[dataLength++] = value[i]; // why not backwards?
      // data[dataLength++] = value[sz-i]; // why backwards?
    data[dataLength++] = '\0';
    while(dataLength & 3) // pad to 4 bytes
      data[dataLength++] = '\0';
    return index;
  }
  void setString(uint8_t index, const char* str){
    setString(index, (uint8_t*)str, strnlen(str, OSC_MESSAGE_MAX_DATA_SIZE-dataLength));
  }
  void setString(uint8_t index, uint8_t* value, size_t sz){
    dataLength = getOffset(index);
    for(size_t i=0; i<sz; ++i)
      data[dataLength++] = value[i];
      // data[dataLength++] = value[sz-i]; // why backwards?
    data[dataLength++] = '\0';
    while(dataLength & 3) // pad to 4 bytes
      data[dataLength++] = '\0';
  }

protected:
  void addType(char type, size_t size){
    prefix[prefixLength++] = type;
    dataLength += size;
  }
  uint8_t add(char type, uint8_t* value){
    prefix[prefixLength++] = type;
    data[dataLength++] = value[3];
    data[dataLength++] = value[2];
    data[dataLength++] = value[1];
    data[dataLength++] = value[0];
    return dataLength-4;
  }
public:
  void setFloat(uint8_t index, float value){
    set32bits(index, (uint8_t*)&value);
  }
  void setInt(uint8_t index, int32_t value){
    set32bits(index, (uint8_t*)&value);
  }
  void set32bits(uint8_t index, uint8_t* value){
    data[index++] = value[3];
    data[index++] = value[2];
    data[index++] = value[1];
    data[index] = value[0];
  }
  void set(uint8_t index, uint8_t* value, size_t sz){
    for(size_t i=1; i<=sz; ++i)
      data[index++] = value[sz-i];
  }
};

#endif /*  __OscMessage_h__ */
