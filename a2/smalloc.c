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
    struct block *temp, *prev, *allocnext, *freecur;
    temp = freelist;
    prev = NULL;
    allocnext = NULL;
    freecur = NULL;

    if (nbytes == 0){ //Cannot smalloc 0 bytes
        return NULL;
    }

    for(; temp!=NULL; prev = temp, temp = temp->next){
        
        if (temp->size == nbytes){

            if (prev == NULL){ //The first free block is the right size
                freecur = freelist->next;
                
                //Add it to head of allocated_list
                allocnext = freelist;
                allocnext->next = allocated_list;
                allocated_list = allocnext;
                
                //Need to remove it from freelist
                freelist = freecur;
                return allocated_list->addr;
            }
            
            //If the size block is not the first block 
            prev->next = temp->next;
            
            allocnext = temp;
            allocnext->next = allocated_list;
            allocated_list = allocnext;
            
            return allocated_list->addr;

        }else if(temp->size > nbytes){ //Found a block bigger than nbytes
            allocated_list = add_to_list(allocated_list, nbytes, temp->addr);

            temp->addr = (allocated_list->addr)+nbytes;
            temp->size =(temp->size)-nbytes;
            
            return allocated_list->addr;
        }
        
    }
    return NULL;
}

int sfree(void *addr) {
    struct block *cur, *prev,*curfree, *curnext;
    cur = allocated_list;
    prev = NULL;
    curfree = NULL;
    curnext = NULL;

    while(cur!= NULL && addr!=NULL){
        
        if ((cur ->addr) == addr){

            if (prev==NULL){
                curnext = allocated_list->next;
                
                curfree = cur;
                curfree->next = freelist;
                freelist = curfree;
                
                allocated_list = curnext; //Allocated loses its head
                return 0;
            }
            
            curnext = cur->next;
            
            curfree = cur;
            curfree->next = freelist;
            freelist = curfree;
            
            prev->next = curnext;
            
            return 0;
        }
        prev = cur;
        cur = cur->next;
    }
    return -1;
}

void mem_init(int size) {
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
    struct block *n_block = NULL;

    while (freelist!=NULL){
        next_block = freelist->next;
        free(freelist);
        freelist = next_block;
    }
    while (allocated_list!=NULL){
        n_block = allocated_list->next;
        free(allocated_list);
        allocated_list = n_block;
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


