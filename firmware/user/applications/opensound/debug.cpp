#include "opensound.h"
#include "application.h"
#include <string.h>

Debug debug;

#if 0
void log_print_(int level, int line, const char *func, const char *file, const char *msg, ...){
  // defined in services/src/debug.c
#ifdef SERIAL_DEBUG
  Serial.println(msg); 
#endif // SERIAL_DEBUG
}
#endif

void debugMessage(const char* msg){
#ifdef SERIAL_DEBUG
  Serial.println(msg); 
#endif // SERIAL_DEBUG
}

char* sitoa(int val, int base){
  const char hexnumerals[] = "0123456789abcdef";
  static char buf[13] = {0};
  int i = 11;
  unsigned int part = abs(val);
  do{
    buf[i--] = hexnumerals[part % base];
    part /= base;
  }while(part && i);
  if(val < 0)
    buf[i--] = '-';
  return &buf[i+1];
}

void assert_failed(const char* msg, const char* location, int line){
  static char buffer[64];
  char* p = buffer;
  p = stpncpy(p, msg, 24);
  p = stpcpy(p, (const char*)" in ");
  p = stpncpy(p, location, 24);
  p = stpcpy(p, (const char*)" line ");
  p = stpcpy(p, sitoa(line, 10));
#ifdef SERIAL_DEBUG
  Serial.println(buffer);
#endif // SERIAL_DEBUG
}
