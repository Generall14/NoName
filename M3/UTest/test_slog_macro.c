#include <SLog/slog.h>
#include "header.h"
#define SLOGNAME_8 3

void test_slog_macro()
{
	LOG_ERROR("fdsafds", 2, 3);
}

void slog_log_entry(uint32_t log_id, ...)
{
	
}
