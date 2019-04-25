#include <typedefs.h>
#include <lib.h>
#include <serial.h>

int main(void){

    serial_init(SERIAL_DEFAULT_DEVICE);
    printf("Hello, world!\n");
    while(1);

    return 0;
}
