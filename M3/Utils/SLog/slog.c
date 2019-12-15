#include "slog.h"
#include "global_clock.h"
#include "stm32f1xx_hal.h"

slog_buff slog_buffer;

void slog_log_entry(uint32_t log_id, ...)
{
	va_list args;
    va_start(args, log_id);

	slog_entry entry;
	entry.log_id = log_id;
	entry.timestamp = get_global_clock_us();

	for(int i=0;i<(log_id & SACOUNT_MASK);i++)
		entry.args[i] = va_arg(args, uint32_t);

	__disable_irq();
	slog_push_entry(&entry, &slog_buffer);
	__enable_irq();
}

void slog_push_entry(slog_entry *entry, slog_buff *buff)
{
	// TODO implementation, utests
}
