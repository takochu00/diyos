#ifndef _SERIAL_H_
#define _SERIAL_H_
#include <typedefs.h>

int serial_init(int index);
int serial_is_send_enable(int index);
int serial_send_byte(int index, uint8_t data);
int serial_is_recv_enable(int index);
uint8_t serial_recv_byte(int index);
//interrupt functions
int serial_intr_is_send_enable(int index);
void serial_intr_send_enable(int index);
void serial_intr_send_disable(int index);
int serial_intr_is_recv_enable(int index);
void serial_intr_recv_enable(int index);
void serial_intr_recv_disable(int index);

#endif//_SERIAL_H_
