#include <typedefs.h>
#include <diyos.h>
#include <lib.h>
#include <interrupt.h>
#include <test.h>

//diy_thread_id_t test09_1_id;
//diy_thread_id_t test09_2_id;
//diy_thread_id_t test09_3_id;

static int start_threads(int argc, char *argv[]){
    //test09_1_id = diy_thread_run(test09_1, "test09_1", 1, 0x100, 0, NULL);
    //test09_2_id = diy_thread_run(test09_2, "test09_2", 2, 0x100, 0, NULL);
    //test09_3_id = diy_thread_run(test09_3, "test09_3", 3, 0x100, 0, NULL);
    diy_thread_run(test10_1, "test10_1", 1, 0x100, 0, NULL);
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
