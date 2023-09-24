#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "heap.h"
/* Here you can see my wonderful implementation of heap :D */

smartbin* bins[63]= {0}; //from 0x8 to 63*0x8
Heap* heap = NULL;
/*help functions */

static size_t align_8(size_t size){
    size_t a = size%8;
    if(a!=0)
        size+=(8-a);
    return size;
}
////////////////////////////
/* MAIN  HEAP FUNCTIONS */
Heap* init(size_t size){

    Heap* heap = mmap(NULL,size,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,0,0);
    if(heap==MAP_FAILED)
        exit(HEAP_INIT_ERR);

    heap->heap_size=size;

    heap->heap_base=heap;

    heap->next_chunk=heap->heap_base+sizeof(Heap);

    heap->current_chunk=heap->next_chunk;

    return heap;
}

wonderful_pointer wonderful_malloc(size_t size){
    
    wonderful_pointer ptr = 0;
    if(heap==NULL)
        heap = init(HEAPSIZE);
 
    if(size>heap->heap_size)
        return ALCERR;
    
    size = align_8(size);
    
    size_t idx  = size/8 -1;

    if(bins[idx]!=NULL && bins[idx]->next!=0){

        ptr = smartbin_get(bins[idx]);

        if(heap->heap_base+ptr > heap->heap_base+heap->heap_size) //security check
            exit(0x100);
        
        return ptr;
    }

    heap->next_chunk->size=size;

    heap->current_chunk=heap->next_chunk;

    ptr=(long)heap->next_chunk - (long)heap->heap_base;

    heap->next_chunk=(chunk*)((long)heap->next_chunk+(long)heap->next_chunk->size+sizeof(size_t)); 

    return ptr+sizeof(size_t);
    //we need to add an offset of size of size_t to our pointer and to next_chunk_ptr caused by size field
}
void wonderful_free(wonderful_pointer ptr){


 
    return;
}
/////////////////////////////////////

///////////////////////
/* SAFE READ AND WRITE FUNCTIONS */

int safe_write(wonderful_pointer pointer,char* buffer,size_t size){
    size_t i =0;

    if(heap->current_chunk->size<size)
        size = heap->current_chunk->size;

    void* real_ptr = heap->heap_base+pointer;

    if(real_ptr > heap->heap_base+heap->heap_size || real_ptr < heap->heap_base)
        return OUT_OF_HEAP;

    for(i=0;i<size;i++)
        *(char*)(real_ptr+i)=buffer[i];

    return i;
}

int safe_read(wonderful_pointer pointer,char* buffer,size_t size){

    size_t i =0;

    void* real_ptr = heap->heap_base+pointer;

    if(heap->current_chunk->size<size) //check for current_chunk oob
        size = heap->current_chunk->size;

    if(real_ptr > heap->heap_base+heap->heap_size || real_ptr<heap->heap_base) //check for heap_oob
        return OUT_OF_HEAP;

    for(i=0;i<size;i++)
        buffer[i]=*(char*)(real_ptr+i);

    return i;
}


/////////////////////////////////////////////
/* SMARTBIN FUNCTIONS */
static smartbin* smartbin_init(size_t size){

    smartbin* bin = mmap(NULL,sizeof(smartbin),PROT_READ | PROT_WRITE,MAP_ANONYMOUS |MAP_PRIVATE,0,0);

    if(bin==MAP_FAILED)
        exit(HEAP_INIT_ERR);

    bin->next = 0;

    bin->size = size;

    return bin;
}
static void smarbin_put(smartbin* bin, wonderful_pointer chunk){

    wonderful_pointer* ptr = heap->heap_base+chunk+sizeof(size_t); //get pointer to freed chunk
    *ptr = bin->next; //write next ptr into chunk;

    bin->next = chunk; //save chunk to bin

    return;
}
static wonderful_pointer smartbin_get(smartbin* bin){

    wonderful_pointer current = bin->next;

    wonderful_pointer next = *(wonderful_pointer*)(heap->heap_base + bin->next+sizeof(size_t));
    //get next chunk addr from next bin field
    bin->next = next;

    return current;
}

