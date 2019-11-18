/**
 * https://github.com/mity/acutest
 * 
 * In one file (main.c) first include <acutest.h>, then "header.h",
 * in other files first include only "header.h" (disabling main
 * function generation).
 * 
 * All test functions have to be declared in header.h and in
 * TEST_LIST.
 * 
 * TODO: some make file.
 */

#include <acutest.h>
#include "header.h"

TEST_LIST = {
   { "test_test1", test_test1 },
   { "test_init_fifo", test_init_fifo },
   { "test_get_head_fifo", test_get_head_fifo },
   { "test_get_tail_fifo", test_get_tail_fifo },
   { NULL, NULL }
};
