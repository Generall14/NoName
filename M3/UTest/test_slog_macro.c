#include <SLog/slog.h>
#include <stdarg.h>
#include "header.h"

void slog_log_entry(uint32_t log_id, ...)
{
}

#define SLOGNAME_13 3

void test_slog_macro()
{
	LOG_ENTRY(1, 2, 3);
}
