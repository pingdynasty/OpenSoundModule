#ifndef __MESSAGE_H
#define __MESSAGE_H

#include <stdint.h>

#define SERIAL_DEBUG

#ifndef CHECKSUM_ERROR_STATUS
#define CHECKSUM_ERROR_STATUS      -10
#endif
#ifndef OUT_OF_MEMORY_ERROR_STATUS
#define OUT_OF_MEMORY_ERROR_STATUS -20
#endif
#ifndef CONFIGURATION_ERROR_STATUS
#define CONFIGURATION_ERROR_STATUS -30
#endif

#ifdef __cplusplus
 extern "C" {
#endif

   void debugMessage(const char* msg);
   // void error(int8_t code, const char* reason);
   // void assert_failed(const char* msg, const char* location, int line);

#ifdef __cplusplus
}

#include "application.h"

class Debug : public Print {
  size_t write(uint8_t data){
#ifdef SERIAL_DEBUG
    return Serial.write(data);
#else
    return 1;
#endif
  }
  size_t write(const uint8_t* data, size_t size){
#ifdef SERIAL_DEBUG
    return Serial.write(data, size);
#else
    return size;
#endif
  }
};
extern Debug debug;

template<class T>
inline Print &operator <<(Print &obj, T arg)
{ obj.print(arg); return obj; }

void debugMessage(const char* msg, int);
void debugMessage(const char* msg, int, int, int);
void debugMessage(const char* msg, float);
void debugMessage(const char* msg, float, float);
void debugMessage(const char* msg, float, float, float);
/* void assert_failed(uint8_t* location, uint32_t line); */

#endif

#endif /* __MESSAG£_H */
