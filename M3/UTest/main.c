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
   { "test_timeout_head", test_timeout_head },
   { "test_push_fifo", test_push_fifo },
   { "test_push_command", test_push_command },
   { "test_spexe_valid_commands", test_spexe_valid_commands },
   { "test_spexe_invalid_commands", test_spexe_invalid_commands },
   { "test_spexe_null_ptrs", test_spexe_null_ptrs },
   { "test_spexe_crc", test_spexe_crc },
   { "test_sprot_read_success", test_sprot_read_success },
   { "test_sprot_read_fail", test_sprot_read_fail },
   { "test_sprot_write_success", test_sprot_write_success },
   { "test_sprot_write_fail", test_sprot_write_fail },
   { NULL, NULL }
};
