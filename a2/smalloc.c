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
    struct block *temp, *prev, *allocnext, *freecur; //Don't want it to directly alter freelist
    temp = freelist;
    prev = NULL;
    allocnext = NULL;
    freecur = NULL;
    
    //Take nbytes from freelist block and create a block of size nbytes and put it in allocated_list.
    for(; temp!=NULL; prev = temp, temp = temp->next){
        
        if (temp->size == nbytes){
            //Found a block of that size
            //allocated_list = add_to_list(allocated_list, nbytes, temp->addr); //Wrong - Don't need to allocate new space.
            //freecur = freelist->next;
            
            //Need to change head of allocated_list to temp
            //allocnext = temp;
            //allocnext->next = allocated_list; //This changes freelist as well!!!!!!!
            //allocated_list = allocnext;

            if (prev == NULL){ //The first free block is the right size
                freecur = freelist->next; //Capture the rest of the LL before it changes from the next few lines
                
                //Add it to head of allocated_list
                allocnext = freelist;
                allocnext->next = allocated_list;
                allocated_list = allocnext;
                
                //Need to remove it from freelist
                freelist = freecur;
                return allocated_list->addr;
            }
            
            //If the size block is not the first block NEED TO TEST THIS AS WELL
            //Capture freelist before and after that block
            prev->next = temp->next;
            
            allocnext = temp;
            allocnext->next = allocated_list; //This changes freelist as well!!!!!!!
            allocated_list = allocnext;
            
            //prev->next = temp->next; //If it's not
            return allocated_list->addr;

        }else if(temp->size > nbytes){
            //Found a block bigger than nbytes
            allocated_list = add_to_list(allocated_list, nbytes, temp->addr); //gGIVING ME ERRORS

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
    curnext=NULL;//Want to capture the previous freelist.
    //This is supposed to be freelist but how can I make it freelist when freelist changes when I give it the new head cur?

    while(cur!= NULL && addr!=NULL){
        
        if ((cur ->addr) == addr){

            if (prev==NULL){ //if it wants to free the first address in allocated_list, then the head of allocated_list becomes cur -> next
                curnext = allocated_list->next; //Hold what the new allocated_list will be
                
                curfree = cur; //which is allocated_list's head
                curfree->next = freelist;
                freelist = curfree;
                
                allocated_list = curnext; //allocated loses its head
                return 0;
            }
            
            curnext = cur->next;
            
            curfree = cur;
            curfree->next = freelist; //But I dont want to change curs value.............
            freelist = curfree;

            
            prev->next = curnext;
            
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


