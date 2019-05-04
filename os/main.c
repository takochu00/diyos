#include <typedefs.h>
#include <diyos.h>
#include <lib.h>
#include <interrupt.h>
#include <test.h>

static int start_threads(int argc, char *argv[]){
    diy_thread_run(test08_1, "command", 0x100, 0, NULL);
    return 0;
}

int main(void){
    INTR_DISABLE;
    printf("diyos booted\n");
    //start DIYOS
    diy_start(start_threads, "start", 0x100, 0, NULL);
    //never come back here.
    return 0;
}
