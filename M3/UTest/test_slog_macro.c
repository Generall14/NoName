#include <SLog/slog.h>
#include "header.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#define SLOGNAME TEST_SLOG_MACRO
bool is_valid_args(uint32_t eargs[]);

#define SKIP 0x12345678

#include "fake/slog_consts.h"

uint32_t pargs[4];

void test_slog_macro()
{
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs1[] = {101, SKIP, SKIP, SKIP};
	LOG_ERROR("fdsafds");
	TEST_CHECK_P(is_valid_args(eargs1), "LOG_ERROR - 0 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs2[] = {103, 104, SKIP, SKIP};
	LOG_ERROR("fdsafds", 104);
	TEST_CHECK_P(is_valid_args(eargs2), "LOG_ERROR - 1 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs3[] = {105, 106, 107, SKIP};
	LOG_ERROR("fdsafds", 106, 107);
	TEST_CHECK_P(is_valid_args(eargs3), "LOG_ERROR - 2 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs4[] = {108, 109, 110, 111};
	LOG_ERROR("fdsafds", 109, 110, 111);
	TEST_CHECK_P(is_valid_args(eargs4), "LOG_ERROR - 3 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs5[] = {201, SKIP, SKIP, SKIP};
	LOG_WARNING("fdsafds");
	TEST_CHECK_P(is_valid_args(eargs5), "LOG_WARNING - 0 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs6[] = {203, 204, SKIP, SKIP};
	LOG_WARNING("fdsafds", 204);
	TEST_CHECK_P(is_valid_args(eargs6), "LOG_WARNING - 1 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs7[] = {205, 206, 207, SKIP};
	LOG_WARNING("fdsafds", 206, 207);
	TEST_CHECK_P(is_valid_args(eargs7), "LOG_WARNING - 2 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs8[] = {208, 209, 210, 211};
	LOG_WARNING("fdsafds", 209, 210, 211);
	TEST_CHECK_P(is_valid_args(eargs8), "LOG_WARNING - 3 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargs9[] = {301, SKIP, SKIP, SKIP};
	LOG_INFO("fdsafds");
	TEST_CHECK_P(is_valid_args(eargs9), "LOG_INFO - 0 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsa[] = {303, 304, SKIP, SKIP};
	LOG_INFO("fdsafds", 304);
	TEST_CHECK_P(is_valid_args(eargsa), "LOG_INFO - 1 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsb[] = {305, 306, 307, SKIP};
	LOG_INFO("fdsafds", 306, 307);
	TEST_CHECK_P(is_valid_args(eargsb), "LOG_INFO - 2 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsc[] = {308, 309, 310, 311};
	LOG_INFO("fdsafds", 309, 310, 311);
	TEST_CHECK_P(is_valid_args(eargsc), "LOG_INFO - 3 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsd[] = {401, SKIP, SKIP, SKIP};
	LOG_DEBUG("fdsafds");
	TEST_CHECK_P(is_valid_args(eargsd), "LOG_DEBUG - 0 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargse[] = {403, 404, SKIP, SKIP};
	LOG_DEBUG("fdsafds", 404);
	TEST_CHECK_P(is_valid_args(eargse), "LOG_DEBUG - 1 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsf[] = {405, 406, 407, SKIP};
	LOG_DEBUG("fdsafds", 406, 407);
	TEST_CHECK_P(is_valid_args(eargsf), "LOG_DEBUG - 2 args");
	
	memset(&pargs, 0x00, sizeof(pargs)/sizeof(pargs[0]));
	uint32_t eargsg[] = {408, 409, 410, 411};
	LOG_DEBUG("fdsafds", 409, 410, 411);
	TEST_CHECK_P(is_valid_args(eargsg), "LOG_DEBUG - 3 args");
}

void MOCK_slog_log_entry(uint32_t log_id, va_list args)
{
	pargs[0] = log_id;

	for(int i=0;i<3;i++)
		pargs[i+1] = va_arg(args, uint32_t);
}

bool is_valid_args(uint32_t eargs[])
{
	if(pargs[0] != eargs[0])
	{
		printf("\nError, log_id = %d, should be %d\n", pargs[0], eargs[0]);
		return false;
	}
	
	for(int i=1;i<4;i++)
	{
		if( (eargs[i] != SKIP) && (eargs[i] != pargs[i]))
		{
			printf("\nError, args[%d] = %d, should be %d\n", i, pargs[i], eargs[i]);
			return false;
		}
	}
	return true;
}
