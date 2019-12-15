#include "slog.h"
#include "global_clock.h"

#ifndef UTEST
#include "stm32f1xx_hal.h"
#endif

slog_buff slog_buffer;

void slog_log_entry(uint32_t log_id, ...)
{
	va_list args;
    va_start(args, log_id);

#ifdef UTEST
	MOCK_slog_log_entry(log_id, args);
	return;
#endif

	slog_entry entry;
	entry.log_id = log_id;
	entry.timestamp = get_global_clock_us();

	for(int i=0;i<(log_id & SACOUNT_MASK);i++)
		entry.args[i] = va_arg(args, uint32_t);

#ifndef UTEST
	__disable_irq();
#endif
	slog_push_entry(&entry, &slog_buffer);
#ifndef UTEST
	__enable_irq();
#endif
}

void slog_push_entry(slog_entry *entry, slog_buff *buff)
{
	// TODO implementation, utests
}

void slog_clear_buff(slog_buff *buff)
{
	//TODO inplementation, utests
}

void slog_init()
{
	slog_clear_buff(&slog_buffer);
}
