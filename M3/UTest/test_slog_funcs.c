#include <SLog/slog.h>
#include "header.h"
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

uint32_t pargs[4];

void test_slog_clear_buff()
{
	slog_buff buff;
	memset(&buff, 0xff, sizeof(slog_buff));
	slog_clear_buff(&buff);
}

