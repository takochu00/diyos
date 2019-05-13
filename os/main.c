#include <typedefs.h>
#include <diyos.h>
#include <lib.h>
#include <interrupt.h>

static int start_threads(int argc, char *argv[]){
    diy_thread_run(console_driver_main, "console_driver", 1, 0x200, 0, NULL);
    diy_thread_run(command_main, "command", 8, 0x200, 0, NULL);
    diy_thread_change_priority(15);//set lowest priority
    INTR_ENABLE;//enable interrupt
    while(1){
        asm volatile ("sleep");//low power mode
    }
    return 0;
}

int main(void){
    INTR_DISABLE;
    printf("diyos boot up\n");
    //start DIYOS
    diy_start(start_threads, "idle", 0, 0x100, 0, NULL);
    //never come back here.
    return 0;
}
