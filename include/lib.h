#ifndef _LIB_H_
#define _LIB_H_
#include <typedefs.h>

int putchar(uint8_t c);
int printf(uint8_t *str);
int putxval(uint32_t value, int column);
void *memset(void *dst, int c, long size);
void *memcpy(void *dst, const void *src, long size);
int memcmp(const void *b1, const void *b2, long size);
int strlen(char *s);
char *strcpy(char *dst, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char*s2, int len);

#endif//_LIB_H_
