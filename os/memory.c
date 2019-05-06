#include <typedefs.h>
#include <diyos.h>
#include <lib.h>
#include <memory.h>

typedef struct diy_mem_block {
    struct diy_mem_block *next;
    int size;
} diy_mem_block;

typedef struct diy_mem_pool {
    int size;
    int num;
    diy_mem_block *free;
} diy_mem_pool;

static diy_mem_pool pool[] = {
    {16, 8, NULL}, {32, 8, NULL}, {64, 4, NULL},
};

#define MEMORY_AREA_NUM (sizeof(pool) / sizeof(*pool))

static int diy_mem_init_pool(diy_mem_pool *p){
    int i;
    diy_mem_block *mp;
    diy_mem_block **mpp;
    extern char freearea;//defined at ld.scr
    static char *area = &freearea;

    mp = (diy_mem_block *)area;
    mpp = &p->free;
    //connect each freearea to linked-list
    for(i = 0; i < p->num; ++i){
        *mpp = mp;
        memset(mp, 0, sizeof(*mp));
        mp->size = p->size;
        mpp = &(mp->next);
        mp = (diy_mem_block *)((char *)mp + p->size);
        area += p->size;
    }
    return 0;
}

int diy_mem_init(void){
    int i;
    for(i = 0; i < MEMORY_AREA_NUM; ++i){
        diy_mem_init_pool(&pool[i]);
    }
    return 0;
}

void *diy_mem_alloc(int size){
    diy_mem_block *mp;
    diy_mem_pool *p;
    int i;
    for(i = 0; i < MEMORY_AREA_NUM; ++i){
        p = &pool[i];
        if(size <= p->size - sizeof(diy_mem_block)){
            if(p->free == NULL){
                //no free space
                diy_sysdown();
                return NULL;
            }
            mp = p->free;
            p->free = p->free->next;
            mp->next = NULL;
            return (void *)(mp + 1);
        }
    }
    //no space to allocate the size
    diy_sysdown();
    return NULL;
}

void diy_mem_free(void *mem){
    int i;
    diy_mem_block *mp;
    diy_mem_pool *p;
    mp = ((diy_mem_block *)mem - 1);
    
    for(i = 0; i < MEMORY_AREA_NUM; ++i){
        p = &pool[i];
        if(mp->size == p->size){
            mp->next = p->free;
            p->free = mp;
            return;
        }
    }

    //no such space is allocated
    diy_sysdown();
}

