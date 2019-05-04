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
