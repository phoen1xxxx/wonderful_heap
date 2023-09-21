#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "heap.h"
Heap* init(size_t size){
    Heap* heap = mmap(NULL,size,PROT_READ | PROT_WRITE,MAP_PRIVATE | MAP_ANONYMOUS,0,0);
    if(heap==MAP_FAILED)
        exit(HEAP_INIT_ERR);
    heap->heap_size=size;

    heap->heap_base=heap;

    heap->next_chunk=heap->heap_base+sizeof(Heap);

    heap->current_chunk=heap->next_chunk;
    return heap;
}

wonderful_pointer wonderful_malloc(Heap* heap,size_t size){
    if(size>heap->heap_size)
        return ALCERR;

    heap->next_chunk->size=size;

    heap->current_chunk=heap->next_chunk;

    wonderful_pointer ptr=(long)heap->next_chunk - (long)heap->heap_base;

    heap->next_chunk=(chunk*)((long)heap->next_chunk+(long)heap->next_chunk->size+sizeof(size_t)); 

    return ptr+sizeof(size_t);
    //we need to add an offset of size of size_t to our pointer and to next_chunk_ptr
}

int safe_write(Heap* heap,wonderful_pointer pointer,char* buffer,size_t size){
    size_t i =0;

    if(heap->current_chunk->size<size)
        size = heap->current_chunk->size;

    void* real_ptr = heap->heap_base+pointer;

    if(real_ptr > heap->heap_base+heap->heap_size || real_ptr<heap->heap_base)
        return OUT_OF_HEAP;

    for(i=0;i<size;i++)
        *(char*)(real_ptr+i)=buffer[i];
    return i;
}

int safe_read(Heap* heap,wonderful_pointer pointer,char* buffer,size_t size){

    size_t i =0;

    void* real_ptr = heap->heap_base+pointer;

    if(heap->current_chunk->size<size)
        size = heap->current_chunk->size;

    if(real_ptr > heap->heap_base+heap->heap_size || real_ptr<heap->heap_base)
        return OUT_OF_HEAP;

    for(i=0;i<size;i++)
        buffer[i]=*(char*)(real_ptr+i);
    return i;
}
