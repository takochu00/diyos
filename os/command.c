#include <typedefs.h>
#include <diyos.h>
#include <console_driver.h>
#include <lib.h>

static void send_use(int index){
    char *p;
    p = diy_thread_kmalloc(3);//init command send buf
    p[0] = '0';
    p[1] = CONSOLE_DRIVER_CMD_USE;
    p[2] = '0' + index;
    diy_thread_msgsend(MSGBOX_ID_CONSOLE_OUTPUT, 3, p);
}

static void send_write(char *str){
    char *p;
    int len = strlen(str);
    p = diy_thread_kmalloc(len + 2);
    p[0] = '0';
    p[1] = CONSOLE_DRIVER_CMD_WRITE;
    memcpy(&p[2], str, len);
    diy_thread_msgsend(MSGBOX_ID_CONSOLE_OUTPUT, len + 2, p);
}

int command_main(int argc, char *argv[]){
    char *p;
    int size;
    send_use(SERIAL_DEFAULT_DEVICE);//request consle_init() to console driver thread

    while(1){
        send_write("command> ");
        diy_thread_msgrecv(MSGBOX_ID_CONSOLE_INPUT, &size, &p);
        p[size] = '\0';
        if(strncmp(p, "echo", 4) == 0){
            send_write(p + 4);
            send_write("\n");
        } else {
            
            send_write("unknown\n");
        }
        diy_thread_kmfree(p);
    }
    return 0;
}
