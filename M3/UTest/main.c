/**
 * https://github.com/mity/acutest
 * 
 * In one file (main.c) first include <acutest.h>, then "header.h",
 * in other files first include only "header.h" (disabling main
 * function generation).
 * 
 * All test functions have to be declared in header.h and in
 * TEST_LIST.
 */

#include <acutest.h>
#include "header.h"

TEST_LIST = {
   { "test_test1", test_test1 },
   { "test_test1", test_test2 },
   { "testXXX", testXXX },
   { NULL, NULL }
};
