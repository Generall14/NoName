#include <SLog/slog.h>
#include "header.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define EXP_TMSTMP 0x12345678
#define A1 0x11111111
#define A2 0x22222222
#define A3 0x33333333
#define A4 0x44444444
#define ID_B 0x12345600
extern uint32_t fake_global_clock_us;

slog_entry passed_entry;

void STUB_slog_push_entry(slog_entry *entry, slog_buff *buff)
{
	memcpy(&passed_entry, entry, sizeof(slog_entry));
}

bool check_passed_slog_entry(slog_entry* expected)
{
	int args = expected->log_id&0x3;
	if(expected->log_id != passed_entry.log_id)
	{
		printf("\nError, log_id: %d, should be: %d\n", passed_entry.log_id, expected->log_id);
		return false;
	}
	if(expected->timestamp != passed_entry.timestamp)
	{
		printf("\nError, timestamp: %d, should be: %d\n", passed_entry.timestamp, expected->timestamp);
		return false;
	}
	for(int i=0;i<args;i++)
	{
		if(expected->args[i] != passed_entry.args[i])
		{
			printf("\nError, args[%d]: %d, should be: %d\n", i, passed_entry.args[i], expected->args[i]);
			return false;
		}
	}
	return true;
}

void test_slog_log_entry()
{
	for(int i=0;i<4;i++)
	{
		// Passed more arguments than in log_id
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+i, A1, A2, A3, A4);
		slog_entry expected = {ID_B+i, EXP_TMSTMP, {i>=1?A1:0, i>=2?A2:0, i>=3?A3:0}};
		TEST_CHECK_P(check_passed_slog_entry(&expected), \
		"Passed more arguments than in log_id (%i).", i);
	}
	
	{
		// Passed 0 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B);
		slog_entry expected = {ID_B, EXP_TMSTMP, {0, 0, 0}};
		TEST_CHECK_P(check_passed_slog_entry(&expected), \
		"Passed 0 arguments.");
	}
	
	{
		// Passed 1 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+1, A1);
		slog_entry expected = {ID_B+1, EXP_TMSTMP, {A1, 0, 0}};
		TEST_CHECK_P(check_passed_slog_entry(&expected), \
		"Passed 1 arguments.");
	}
	
	{
		// Passed 2 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+2, A1, A2);
		slog_entry expected = {ID_B+2, EXP_TMSTMP, {A1, A2, 0}};
		TEST_CHECK_P(check_passed_slog_entry(&expected), \
		"Passed 2 arguments.");
	}
	
	{
		// Passed 3 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+3, A1, A2, A3);
		slog_entry expected = {ID_B+3, EXP_TMSTMP, {A1, A2, A3}};
		TEST_CHECK_P(check_passed_slog_entry(&expected), \
		"Passed 3 arguments.");
	}
}

