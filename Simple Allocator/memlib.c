#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include "memlib.h"

#define MAX_HEAP(20*(1<<20))//20 MB

static char *mem_heap;
static char *mem_brk;
static char *mem_max_addr;

void mem_init(void)
{
    mem_heap = (char *)malloc(MAX_HEAP);
    mem_brk = (char *)mem_heap;
    mem_max_addr = (char *)(mem_heap + MAX_HEAP);
}

void *mem_sbrk(int incr)
{
    char *old_brk = mem_brk;
    if((incr<0) || ((mem_brk + incr)>mem_max_addr)){
        errno = ENOMEM;
        fprintf(stderr, "error: mem_sbrk failed. Ran out of memory...\n");
        return (void *)-1;
    }
    mem_brk += incr;
    return (void *)old_brk;
}

void mem_deinit(void)
{
    
}

void mem_reset_brk()
{
    mem_brk = （char *)mem_heap;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void *mem_heap_lo()
{
    return (void *)mem_heap;
}

/* 
 * mem_heap_hi - return address of last heap byte
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize() 
{
    return (size_t)((void *)mem_brk - (void *)mem_heap);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}

















