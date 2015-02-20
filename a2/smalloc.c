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
    //Take nbytes from freelist block and create a block of size nbytes and put it in allocated_list.
    for(; freelist!=NULL; freelist = freelist->next){
        if (freelist->size == nbytes){
            //Found a block of that size
            allocated_list = add_to_list(allocated_list, nbytes, freelist->addr);
            return allocated_list;

        }else if(freelist->size > nbytes){
            //Found a block bigger than nbytes
            allocated_list = add_to_list(allocated_list, nbytes, freelist->addr);
            return allocated_list;

            //Creates a new block and then points it to what freelist previously was pointing to
            struct block *f = {(allocated_list->addr)+nbytes, (freelist->size)-nbytes, freelist->next};
            freelist = f;
        }
    }
    return NULL;
    
}

int sfree(void *addr) {
    struct block *cur, *prev;

    for (cur = allocated_list, prev = NULL;
        cur != NULL && cur->addr != addr;
        prev = cur, cur = cur->next){
        if (cur == NULL){
            return -1;
        }if (prev==NULL){
            allocated_list = allocated_list->next; 
        }else{
            //Catches the memory address released from allocated to freelist
            freelist = add_to_list(freelist, sizeof(cur), cur->addr);
            prev->next = cur->next; //Will this alter allocated_list directly?
        }
    }
    return 0;
    
}

void mem_init(int size) { //COMPLETE
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

    allocated_list = NULL; 

    //Add first block to freelist. So freelist has one mmap sized block.
    freelist = malloc(sizeof(mem)); //Should only use malloc here?
    freelist->addr = mem;
    freelist->size = sizeof(mem);
    freelist->next = NULL;
}

void mem_clean(){
    for(; freelist!=NULL; freelist = freelist->next){
        free(freelist);
    }
    for(; allocated_list!=NULL; allocated_list = allocated_list->next){
        free(allocated_list);
    }
}

/* Adds to list
*/
struct block *add_to_list(struct block *list, int sz, void *address){
    
    struct block *new_node = NULL;

    //new_node = malloc(sizeof(struct block));

    new_node->addr = address;
    new_node->size = sz;
    new_node->next = list;

    return new_node;
}

