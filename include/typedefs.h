#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_

#define NULL ((void *)0)
#define SERIAL_DEFAULT_DEVICE 1

typedef unsigned char   uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long  uint32_t;
typedef char             int8_t;
typedef short           int16_t;
typedef long            int32_t;

typedef uint32_t diy_thread_id_t;
typedef int (*diy_func_t)(int argc, char *argv[]);
typedef void (*diy_handler_t)(void);

typedef enum {
    MSGBOX_ID_CONSOLE_INPUT = 0,
    MSGBOX_ID_CONSOLE_OUTPUT,
    MSGBOX_ID_NUM,
} diy_msgbox_id_t;

#endif//_TYPEDEFS_H_
