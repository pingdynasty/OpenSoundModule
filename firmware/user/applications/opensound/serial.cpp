#include "serial.h"
#include "application.h"

void serial_setup(uint32_t baudRate){
  Serial1.begin(baudRate);
}

void serial_read(uint8_t* data, uint16_t size){
  while(Serial1.available() < size);
  for(int i=0; i<size; ++i)
    data[i] = (uint8_t)Serial1.read();
}

void serial_write(uint8_t* data, uint16_t size){
  Serial1.write(data, size);
}
