#include <SProt/sprot_l.h>
#include "header.h"

#define VAL 5

void test_test1()
{
	int a = VAL;
	
	TEST_CHECK(a==VAL);
	
	if(!TEST_CHECK(a==VAL)) {
        TEST_MSG("a: %d", a);
    }
    
    TEST_CHECK_P(a == VAL, "a: %d", a);
}
