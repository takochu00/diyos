#include <typedefs.h>
#include <lib.h>
#include <serial.h>

static int init(void){
    /* refer symbols defined at ld.scr */
    extern int erodata, data_start, edata, bss_start, ebss;

    /*initialize data area */
    memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
    /*initialize bss area */
    memset(&bss_start, 0, (long)&ebss - (long)&bss_start);
    serial_init(SERIAL_DEFAULT_DEVICE);

    printf("system initialized\n");
    return 0;
}

int g_data = 0x10;
int g_bss;
static int s_data = 0x20;
static int s_bss;

static void printvalues(void){
    printf("g_data = "); putxval(g_data, 0); putchar('\n');
    printf("g_bss  = "); putxval(g_bss, 0); putchar('\n');
    printf("s_data = "); putxval(s_data, 0); putchar('\n');
    printf("s_bss  = "); putxval(s_bss, 0); putchar('\n');
}

int main(void){
    init();

    printvalues();
    printf("change values\n");
    g_data = 0x11;
    g_bss = 0x12;
    s_data = 0x21;
    s_bss = 0x22;
    printvalues();
    while(1);

    return 0;
}
