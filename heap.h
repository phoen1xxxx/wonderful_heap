#include <stdio.h>
#include <stdlib.h>
#define HEAP_INIT_ERR 0xde
#define HEAPSIZE 4096*100 //0x64000
#define ALCERR -1
#define OUT_OF_HEAP -2
typedef unsigned int wonderful_pointer;

typedef struct chunk{
    size_t size;
    //if freed
    wonderful_pointer fd_offset;
}chunk;

typedef struct smartbin{
    size_t size;
    wonderful_pointer next;
}smartbin;

typedef struct Heap{
    size_t heap_size;
    void* heap_base;
    chunk* next_chunk;
    chunk* current_chunk;
}Heap;

wonderful_pointer wonderful_malloc(Heap* heap,size_t size);

void wonderful_free(wonderful_pointer pointer);

Heap* init(size_t size);

int safe_write(Heap* heap,wonderful_pointer pointer,char* buffer,size_t size);

int safe_read(Heap* heap,wonderful_pointer pointer,char* buffer,size_t size);

static void smartbin_put(Heap* heap,smartbin smartbin,wonderful_pointer chunk);

static wonderful_pointer smartbin_get(Heap* heap,smartbin smartbin);
