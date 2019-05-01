#ifndef _SERIAL_H_
#define _SERIAL_H_
#include <typedefs.h>

int serial_init(int index);
int serial_is_send_enablet(int index);
int serial_send_byte(int index, uint8_t data);
int serial_is_recv_enable(int index);
uint8_t serial_recv_byte(int index);

#endif//_SERIAL_H_
