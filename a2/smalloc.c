#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem; 
struct block *freelist; 
struct block *allocated_list; 

void *smalloc(unsigned int nbytes) {
    struct block *temp, *prev; //Don't want it to directly alter freelist
    temp = freelist;
    prev = NULL;
    
    //Take nbytes from freelist block and create a block of size nbytes and put it in allocated_list.
    for(; temp!=NULL; prev = temp, temp = temp->next){
        
        if (temp->size == nbytes){
            //Found a block of that size
            allocated_list = add_to_list(allocated_list, nbytes, temp->addr);
            
            //Need to remove it from freelist
            if (prev == NULL){ //The first free block is the right size
                freelist = temp->next;
                return allocated_list->addr;
            }
            
            prev->next = temp->next; //If it's not
            return allocated_list->addr;

        }else if(temp->size > nbytes){
            //Found a block bigger than nbytes
            allocated_list = add_to_list(allocated_list, nbytes, temp->addr);

            temp->addr = (allocated_list->addr)+nbytes;
            temp->size =(temp->size)-nbytes;
            
            return allocated_list->addr;
        }
        
    }
    return NULL;
}

int sfree(void *addr) {
    struct block *cur, *prev;
    cur = allocated_list;
    prev = NULL;

    while(cur!= NULL && addr!=NULL){
        
        if ((cur ->addr) == addr){
            freelist = add_to_list(freelist, cur->size, cur->addr);
            if (prev==NULL){ //if it wants to free the first address in allocated_list, then the head of allocated_list becomes cur -> next
                allocated_list = cur->next;
                return 0;
            }
            prev->next = cur->next;
            
            return 0;
        }
        
        prev = cur;
        cur = cur->next;
        
    }
    
    return -1;

    
}

void mem_init(int size) { //COMPLETE
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

    allocated_list = NULL; 

    freelist = malloc(sizeof(struct block));
    freelist->addr = mem;
    freelist->size = size;
    freelist->next = NULL;
}

void mem_clean(){

	struct block *next_block = NULL;

while (freelist!=NULL){
	next_block = freelist->next;
	free(freelist);
	freelist = next_block;

}


next_block = NULL;
while (allocated_list!=NULL){
	next_block = allocated_list->next;
	free(allocated_list);
	allocated_list = next_block;

}

}

/* Adds to list */
struct block *add_to_list(struct block *list, int sz, void *address){
    
    struct block *new_node = NULL;

    new_node = malloc(sizeof(struct block));

    
    new_node->addr = address;
    new_node->size = sz;
    new_node->next = list;

    return new_node;
}

