#ifndef _MEMORY_H_
#define _MEMORY_H_

int diy_mem_init(void);
void *diy_mem_alloc(int size);
void diy_mem_free(void *mem);

#endif//_MEMORY_H_
