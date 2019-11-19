#ifndef _HEADER_H_
#define _HEADER_H_

#define TEST_NO_MAIN
#include <acutest.h>
#include <stdio.h>

#define TEST_CHECK_P(VAL, ...) if(!TEST_CHECK(VAL)){printf(__VA_ARGS__);printf("\n");}

void test_test1();
void test_init_fifo();
void test_get_head_fifo();
void test_get_tail_fifo();
void test_timeout_head();
void test_push_fifo();
#endif
