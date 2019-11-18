#include <SProt/sprot_l.h>
#include "header.h"

void test_test1()
{
	int a = 5;
	TEST_CHECK(a==5);
	
	if(!TEST_CHECK(a == 5)) {
        TEST_MSG("a: %d", a);
    }
}

void test_test2()
{
}
