#include "header.h"



void test_slog_storage()
{
	int *p1 = malloc(10*sizeof *p1);
    printf("default-aligned addr:   %p\n", (void*)p1);
	TEST_CHECK_P(((unsigned long)p1)&0x3FF == 0, "malloc - not aligned");
    free(p1);
 
    int *p2 = aligned_alloc(1024, 1024*sizeof *p2);
    printf("1024-byte aligned addr: %p\n", (void*)p2);
	unsigned long align = ((unsigned long)p2)&0x3FF;
	TEST_CHECK_P(align == 0, "aligned_alloc - not aligned");
    free(p2);
}
