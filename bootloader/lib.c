#include <typedefs.h>
#include <serial.h>
#include <lib.h>

int putchar(uint8_t c){
    if(c == '\n') serial_send_byte(SERIAL_DEFAULT_DEVICE, '\r');
    return serial_send_byte(SERIAL_DEFAULT_DEVICE, c);
}

int printf(uint8_t *str){
    while(*str){
        putchar(*(str++));
    }
    return 0;
}

int putxval(uint32_t value, int column){
    char buf[9]; // buffer to print
    char *p; //pointer to operate each column value

    p = buf + sizeof(buf) -1; //process from bottom value
    *(p--) = '\0';//null character

    if(!value && !column) ++column;

    while(value || column){
        *(p--) = "0123456789abcdef"[value & 0xf];
        value >>= 4; //shift to next digit
        if(column) --column;
    }

    return printf(p+1);
}

uint8_t getchar(void){
    uint8_t c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    c = (c == '\r') ? '\n' : c; //return code translation.
    putchar(c); //echo back
    return c;
}

int getline(uint8_t *buf){
    int i = 0;
    uint8_t c;
    do {
        c = getchar();
        if(c == '\n') c = '\0';
        buf[i++] = c;
    } while(c);
    return i - 1;
}



void *memset(void *dst, int c, long size){
    uint8_t *p;
    for(p = (uint8_t *)dst; size > 0; --size){
        *(p++) = c;
    }
    return dst;
}
void *memcpy(void *dst, const void *src, long size){
    uint8_t *d = dst;
    const uint8_t *s = src;
    for(; size > 0; --size){
        *(d++) = *(s++);
    }
    return dst;
}
int memcmp(const void *b1, const void *b2, long size){
    const uint8_t *p1 = (const uint8_t *)b1;
    const uint8_t *p2 = (const uint8_t *)b2;
    for(; size > 0; --size){
        if(*p1 > *p2) return 1;
        else if(*p1 < *p2) return -1;//compiler warns -1 is 64bit on my environemnt
        ++p1;
        ++p2;
    }
    return 0;
}

int strlen(char *s){
    int len;
    for(len = 0; *s; ++s, ++len);
    return len;
}
char *strcpy(char *dst, const char *src){
    char *d;
    for(d = dst;; ++d, ++src){
        *d = *src;
        if(!*src) break; //'\0'
    }
    return dst;
}

int strcmp(const char *s1, const char *s2){
    for(; *s1 || *s2; ++s1, ++s2){
        if(*s1 > *s2) return 1;
        else if(*s1 < *s2) return -1;//compiler warns -1 is 64bit on my environemnt
    }
    return 0;
}

int strncmp(const char *s1, const char*s2, int len){
    for(;(len > 0) && (*s1 || *s2); --len, ++s1, ++s2){
        if(*s1 > *s2) return 1;
        else if(*s1 < *s2) return -1;//compiler warns -1 is 64bit on my environemnt
    }
    return 0;
}


