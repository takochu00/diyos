#include <typedefs.h>
#include <serial.h>
#include <lib.h>

int putchar(uint8_t c){
    if(c == '\n') serial_send_byte(SERIAL_DEFAULT_DEVICE, '\r');
    return serial_send_byte(SERIAL_DEFAULT_DEVICE, c);
}

int printf(uint8_t *str){
    while(*str){
        putchar(*(str++));
    }
    return 0;
}
