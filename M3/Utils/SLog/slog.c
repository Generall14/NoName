#include "slog.h"
#include "global_clock.h"

void slog_log_entry(uint32_t log_id, ...)
{
	va_list args;
    va_start(args, log_id);

	slog_entry entry;
	entry.log_id = log_id;
	entry.timestamp = get_global_clock_us();

	for(int i=0;i<log_id & SACOUNT_MASK;i++)
		entry.args[i] = va_arg(args, uint32_t);

	//TODO: implementation - push entry
}
