#include "slog.h"
#include "global_clock.h"
#include "utils.h"
#include <assert.h>
#include <string.h>

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

	_GID;
	STUB(slog_push_entry(&entry, &slog_buffer));
	_GIE;
}

void slog_push_entry(slog_entry *entry, slog_buff *buff)
{
	assert(buff->head <= SLOG_BUFF_BYTES);

	if(buff->data_lost)
		return;

	uint8_t required = ((entry->log_id&SACOUNT_MASK) + 2)*4;
	uint8_t available = SLOG_BUFF_BYTES - buff->head;
	//printf("\nrequired %d, available %d, head %d\n", required, available, buff->head);
	if(required > available)
	{
		buff->data_lost = 0x01;
		return;
	}

	memcpy(&(buff->data[buff->head]), entry, required);
//	uint32_t* ptr = (uint32_t*)&(buff->data[buff->head]);
//	*ptr = entry->log_id;
//	*ptr++ = (uint32_t)0x66;
//	*ptr++ = (uint32_t)0x66;
//	*ptr++ = (uint32_t)0x66;
//	*ptr++ = (uint32_t)0x66;
//	*ptr++ = (uint32_t)0x66;
	buff->head += required;
	// TODO implementation
}

void slog_clear_buff(slog_buff *buff)
{
	buff->data_lost = 0;
	buff->head = 0;
}

void slog_init()
{
	slog_clear_buff(&slog_buffer);
}
