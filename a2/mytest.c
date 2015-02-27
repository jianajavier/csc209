#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64 //262 144


/* This is an interesting test case because it tests the edge cases
 * for sfree and smalloc. It tests smalloc on an exact size of a free 
 * block if it is at the head of freelist and for 
 * a size that is larger than the head of freelist, but smaller than
 * the next block in freelist. Then it tests sfree on a NULL pointer,
 * a pointer that doesn't exist in allocated_list, and the head of the
 * allocated_list.
 */

int main(void) {
    
    mem_init(SIZE);
    
    char *ptrs[8];
    int i;
    
    printf("After initial call to mem_init()\n");
    printf("Allocated blocks:\n");
    print_allocated(); 
    printf("Free blocks:\n");
    print_free();
    
    
    ptrs[0] = smalloc(200000);
    ptrs[1] = smalloc(60000);
    ptrs[2] = smalloc(2100);

    //Should not work so ptrs[3] = NULL 
    ptrs[3] = smalloc(45);
    
    //Allocating it to free it
    ptrs[4] = smalloc(20);
    sfree(ptrs[4]); //Now points to a block in freelist

    //Will be used later
    ptrs[5] = NULL;
    ptrs[6] = NULL;
    ptrs[7] = NULL;
    
    
    write_to_mem(200000, ptrs[0], 0);
    write_to_mem(60000, ptrs[1], 1);
    write_to_mem(2100, ptrs[2], 2);
    
    printf("\nsmalloc results:\n");
    for (i = 0; i < 8; i++) {
        printf("ptrs[%d] = %p\n", i, ptrs[i]);
    }
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free(); 
    
    printf("\nResults of sfree on head in allocated_list (and then smallocing again):\n");
    printf("Expected = 0; Actual = %d\n", sfree(ptrs[2]));
    ptrs[2] = smalloc(2100);
    
    printf("\nResults of sfree on an address not in allocated_list:\n");
    printf("Expected = -1; Actual = %d\n", sfree(ptrs[4]));
    
    printf("\nResults of sfree on NULL:\n");
    printf("Expected = -1; Actual = %d\n", sfree(ptrs[3]));
    
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free(); //Prints two blocks
    
    printf("\nsmalloc-ing exactly 20 bytes");
    ptrs[3] = smalloc(20); //Since it was NULL before, it shouldn't matter
    printf("\nptrs[3] = %p\n", ptrs[3]);
    
    printf("\nFreeing what we just smalloc-ed and smalloc-ing 21 bytes");
    sfree(ptrs[3]);
    ptrs[5] = smalloc(21);
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();
    
    printf("\nsmallocing the remaining exactly 20 bytes");
    ptrs[6] = smalloc(20);
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();
    
    printf("smalloc results:\n");
    for (i = 0; i < 8; i++) {
        printf("ptrs[%d] = %p\n", i, ptrs[i]); //ptrs[3], ptrs[4], ptrs[6] should be equal.
    }

    sfree(ptrs[0]);
    sfree(ptrs[1]);

    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();
    ptrs[0] = smalloc(200000);
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();


    mem_clean();
    
    printf("\nTesting mem_clean\n");
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("\nFree blocks:\n");
    print_free();
    
    ptrs[7] = smalloc(1); //Should be NULL because cleaned
    printf("\nResults of trying to smalloc after mem_clean\n");
    printf("ptrs[7] = %p\n", ptrs[7]);
    
    return 0;
}

