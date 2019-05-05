#include <test.h>
#include <typedefs.h>
#include <diyos.h>
#include <lib.h>

int test08_1(int argc, char *argv[]){
    char buf[32];
    printf("test08_1 started.\n");
    while(1){
        printf("> ");
        getline(buf);
        if(strncmp(buf, "echo", 4) == 0){
            printf(buf + 4);
            putchar('\n');
        }
        else if(strncmp(buf, "exit", 4) == 0){
            break;
        }
        else {
            printf("unknown cmd.\n");
        }

    }
    printf("test08_1 exit.\n");
    return 0;
}

int test09_1(int argc, char *argv[]){
    printf("test09_1 start\n");
    printf("test09_1 sleep in\n");
    diy_thread_sleep();
    printf("test09_1 sleep out\n");

    printf("test09_1 change priority in\n");
    diy_thread_change_priority(3);//1 -> 3
    printf("test09_1 change priority out\n");
    
    printf("test09_1 wait in\n");
    diy_thread_wait();
    printf("test09_1 wait out\n");

    printf("test09_1 trap in\n");
    asm volatile ("trapa #1");
    printf("test09_1 trap out\n");

    printf("test09_1 exit\n");
    return 0;
}

int test09_2(int argc, char *argv[]){
    printf("test09_2 start\n");
    printf("test09_2 sleep in\n");
    diy_thread_sleep();
    printf("test09_2 sleep out\n");

    printf("test09_2 change priority in\n");
    diy_thread_change_priority(3);//2 -> 3
    printf("test09_2 change priority out\n");
    
    printf("test09_2 wait in\n");
    diy_thread_wait();
    printf("test09_2 wait out\n");

    printf("test09_2 exit\n");
    return 0;
}

int test09_3(int argc, char *argv[]){
    printf("test09_3 start\n");
    printf("test09_3 wakeup in (09_1)\n");
    diy_thread_wakeup(test09_1_id);
    printf("test09_3 wakeup out\n");

    printf("test09_3 wakeup in (09_2)\n");
    diy_thread_wakeup(test09_2_id);
    printf("test09_3 wakeup out\n");
    
    printf("test09_3 wait in\n");
    diy_thread_wait();
    printf("test09_3 wait out\n");

    printf("test09_3 exit in\n");
    diy_thread_exit();
    printf("test09_3 exit out\n");
    return 0;
}
