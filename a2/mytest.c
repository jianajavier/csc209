#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64 //262 144


/* This is an interesting test case because it tests the edge cases
 * for sfree and smalloc. It tests smalloc on an exact size of a free 
 * block if it is at the head of freelist, and if it is not. It also 
 * tests smalloc for a size that is larger than the head of freelist, 
 * but smaller than the next block in freelist. It also tests sfree
 * on a NULL pointer, a pointer that doesn't exist in allocated_list
 * and the head of the allocated_list.
 */

int main(void) {
    
    mem_init(SIZE);
    
    char *ptrs[7];
    int i;
    
    printf("After initial call to mem_init()\n");
    printf("Allocated blocks:\n");
    print_allocated(); //Should we print the actual block values
    printf("Free blocks:\n");
    print_free(); //Prints null
    
    
    ptrs[0] = smalloc(200000);
    ptrs[1] = smalloc(60000);
    ptrs[2] = smalloc(2100);
    ptrs[3] = smalloc(45); //Should be NULL
    
    ptrs[6] = smalloc(20); //Allocating it to free it
    sfree(ptrs[6]); //It just goes to freelist right, but it still has a memory address?
    
    
    write_to_mem(200000, ptrs[0], 0);
    write_to_mem(60000, ptrs[1], 1);
    write_to_mem(2100, ptrs[2], 2);
    
    printf("\nsmalloc results:\n");
    for (i = 0; i < 7; i++) {
        printf("ptrs[%d] = %p\n", i, ptrs[i]); //Last 4 should be null.. Should I check if the other ones correspond with the allocated_list values?
    }
    
    printf("\nAllocated blocks:\n");
    print_allocated(); //Should we print the actual block values
    printf("Free blocks:\n");
    print_free(); //Prints null
    
    printf("\nResults of sfree on head in allocated_list (and then smallocing again):\n");
    printf("Expected = 0; Actual = %d\n", sfree(ptrs[2]));
    ptrs[2] = smalloc(2100);
    
    printf("\nResults of sfree on an address not in allocated_list:\n");
    printf("Expected = -1; Actual = %d\n", sfree(ptrs[6]));
    
    printf("\nResults of sfree on NULL:\n");
    printf("Expected = -1; Actual = %d\n", sfree(ptrs[3]));
    
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free(); //Prints two blocks
    
    printf("\nsmalloc-ing exactly 20 bytes");
    ptrs[3] = smalloc(20); //Since it was NULL before, it shouldn't matter - Check if this takes from the 20 000 block or from the actual 44 block. Which one do we want it to take from..?
    printf("\nptrs[3] = %p\n", ptrs[3]);
    
    printf("\nFreeing what we just smalloc-ed and smalloc-ing 21 bytes"); //Checks if it loops correctly through freelist, and correctly takes from the next block. But it's less than exact
    sfree(ptrs[3]); //But can ptrs[3] still point to the same address?
    ptrs[4] = smalloc(21);
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();
    
    printf("\nsmallocing the remaining exactly 20 bytes"); //Oh it just takes from the 20 -- If I do 3, then it just takes from the 20 and makes it 17.
    ptrs[6] = smalloc(20); //ptrs[6] has an address already, but it's been claimed by freelist
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks\n");
    print_free();
    
    
    printf("smalloc results:\n");
    for (i = 0; i < 7; i++) {
        printf("ptrs[%d] = %p\n", i, ptrs[i]);
    }
    
    mem_clean();
    
    printf("\nTesting mem_clean\n");
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free();
    
    ptrs[5] = smalloc(1); //Should not work because cleaned
    printf("\nResults of trying to smalloc after mem_clean\n");
    printf("ptrs[5] = %p\n", ptrs[5]);
    
    return 0;
}
