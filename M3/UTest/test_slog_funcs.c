#include <SLog/slog.h>
#include "header.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

void test_slog_clear_buff()
{
	slog_buff buff;
	memset(&buff, 0xff, sizeof(slog_buff));
	slog_clear_buff(&buff);
	TEST_CHECK_P(buff.head==0, "buff.head should be zero.");
	TEST_CHECK_P(buff.data_lost==0, "buff.data_lost should be zero.");
}

