#include <typedefs.h>
#include <lib.h>
#include <serial.h>

int main(void){

    printf("diyos loaded\n");

    static char buf[32];
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
            printf("unknown cmd\n");
        }
    }

    return 0;
}
