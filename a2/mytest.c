#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64 //262 144


/* This is an interesting test case because it tests edge cases
 * for sfree and smalloc. It tests smalloc on an exact size of a free 
 * block that is larger than the head of freelist to see if it iterates 
 * freelist properly and smallocs an exact number of bytes correctly. 
 * Then it tests sfree on a pointer that doesn't exist in allocated_list, 
 * but is in freelist. It also smallocs all the memory in freelist.
 */

int main(void) {
    
    mem_init(SIZE);
    
    char *ptrs[4];
    int i;
    
    printf("After initial call to mem_init()\n");
    printf("Allocated blocks:\n");
    print_allocated(); 
    printf("Free blocks:\n");
    print_free();
    
    ptrs[0] = smalloc(200000);
    ptrs[1] = smalloc(60000);
    ptrs[2] = smalloc(2100);

    ptrs[3] = smalloc(44);
    sfree(ptrs[3]);
    
    write_to_mem(200000, ptrs[0], 0);
    write_to_mem(60000, ptrs[1], 1);
    write_to_mem(2100, ptrs[2], 2);
    
    printf("\n4 smalloc calls results:\n");
    for (i = 0; i < 4; i++) {
        printf("ptrs[%d] = %p\n", i, ptrs[i]);
    }
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free(); 
    
    printf("Freeing ptrs[1] and ptrs[2] and ptrs[3]\n");
    printf("Expect: 0, Actual: %d\n", sfree(ptrs[1]));
    printf("Expect: 0, Actual: %d\n", sfree(ptrs[2]));
    printf("Expect: -1, Actual: %d\n", sfree(ptrs[3]));
    
    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free();

    printf("smalloc-ing exactly 60000 bytes to ptrs[3]");
    ptrs[3] = smalloc(60000);

    printf("\nAllocated blocks:\n");
    print_allocated();
    printf("Free blocks:\n");
    print_free();

    printf("\nptrs:\n");
        for (i = 0; i < 4; i++) {
            printf("ptrs[%d] = %p\n", i, ptrs[i]);
        } 

    mem_clean();
    
    printf("\nTesting mem_clean\n");
    printf("\nAllocated blocks:");
    print_allocated();
    printf("\nFree blocks:");
    print_free();
    
    return 0;
}

