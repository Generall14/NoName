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

void test_slog_log_entry()
{
	for(int i=0;i<4;i++)
	{
		// Passed more arguments than in log_id
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+i, A1, A2, A3, A4);
		slog_entry expected = {ID_B+i, EXP_TMSTMP, {i>=1?A1:0, i>=2?A2:0, i>=3?A3:0}};
		TEST_CHECK_P(memcmp(&expected, &passed_entry, sizeof(slog_entry))==0, \
		"Passed more arguments than in log_id (%i).", i);
	}
	
	{
		// Passed 0 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B);
		slog_entry expected = {ID_B, EXP_TMSTMP, {0, 0, 0}};
		TEST_CHECK_P(memcmp(&expected, &passed_entry, sizeof(slog_entry))==0, \
		"Passed 0 arguments.");
	}
	
	{
		// Passed 1 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+1);
		slog_entry expected = {ID_B+1, EXP_TMSTMP, {A1, 0, 0}};
		TEST_CHECK_P(memcmp(&expected, &passed_entry, sizeof(slog_entry))==0, \
		"Passed 1 arguments.");
	}
	
	{
		// Passed 2 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+2);
		slog_entry expected = {ID_B+2, EXP_TMSTMP, {A1, A2, 0}};
		TEST_CHECK_P(memcmp(&expected, &passed_entry, sizeof(slog_entry))==0, \
		"Passed 2 arguments.");
	}
	
	{
		// Passed 3 arguments
		fake_global_clock_us = EXP_TMSTMP;
		memset(&passed_entry, 0x00, sizeof(slog_entry));
		slog_log_entry(ID_B+3);
		slog_entry expected = {ID_B+3, EXP_TMSTMP, {A1, A2, A3}};
		TEST_CHECK_P(memcmp(&expected, &passed_entry, sizeof(slog_entry))==0, \
		"Passed 3 arguments.");
	}
}

