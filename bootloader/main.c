#include <typedefs.h>
#include <lib.h>
#include <serial.h>

int main(void){

    serial_init(SERIAL_DEFAULT_DEVICE);
    printf("Hello, world!\n");

    putxval(0x10, 0);
    putchar('\n');
    putxval(0xffff, 0);
    putchar('\n');
    while(1);

    return 0;
}
