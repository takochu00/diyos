#include <typedefs.h>
#include <lib.h>
#include <serial.h>
#include <xmodem.h>
#include <elf.h>
#include <interrupt.h>

static int init(void){
    //refer symbols defined at ld.scr
    extern int erodata, data_start, edata, bss_start, ebss;

    //initialize data area
    memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
    //initialize softvec for interrupt
    softvec_init();
    //initialize bss area
    memset(&bss_start, 0, (long)&ebss - (long)&bss_start);
    serial_init(SERIAL_DEFAULT_DEVICE);

    printf("system initialized\n");
    return 0;
}

//dump buffers
static int dump(char *buf, long size){
    long i;
    if(size < 0){
        printf("No Data\n");
        return -1;
    }
    for(i = 0; i < size; ++i){
        putxval(buf[i], 2);
        if((i & 0xF) == 0xF){
            putchar('\n');
        } else {
            if((i & 0xF) == 0x7) putchar(' ');
            putchar(' ');
        }
    }
    putchar('\n');
    return 0;
}

static void wait(void){
    volatile long i;
    const long wait_time = 300000;
    for(i = 0; i < wait_time; ++i);
}

int main(void){
    static char buf[16];
    static long size = -1;
    static uint8_t *loadbuf = NULL;
    extern int buffer_start; //defined at linker script
    //initialize data/bss area and serial device
    INTR_DISABLE;//Disable interrupt at first
    init();

    printf("bootloader start\n");
    while(1){
        printf("bootloader> ");
        getline(buf);
        if(strcmp(buf, "load") == 0){
            loadbuf = (uint8_t *)&buffer_start;
            size = xmodem_recv(loadbuf);
            wait();
            if(size < 0){
                printf("XMODEM Receive Error\n");
            } else {
                printf("XMODEM Receive Success\n");
            }
        }
        else if(strcmp(buf, "dump") == 0){
            printf("size: ");
            putxval(size, 0);
            putchar('\n');
            dump(loadbuf, size);
        }
        else if(strcmp(buf, "run") == 0){
            uint8_t *entry_point = elf_load(loadbuf);
            if(entry_point == NULL){
                printf("run error!\n");
            }
            else {
                printf("starting from entry_point: ");
                void (*f)(void);
                putxval((uint32_t)entry_point, 0);
                putchar('\n');
                f = (void (*) (void))entry_point;
                f();
            }
        }
        else {
            printf("unknown cmd\n");
        }
    }
    return 0;
}
