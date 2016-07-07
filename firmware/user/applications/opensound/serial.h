#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif
   void serial_setup(uint32_t baudRate);
   void serial_read(uint8_t* data, uint16_t size);
   void serial_write(uint8_t* data, uint16_t size);
#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H */
