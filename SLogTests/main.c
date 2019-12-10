#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "tst.h"
#undef SLOGNAME
#define SLOGNAME MAIN_FILE

void slog_log(uint32_t log_id, ...)
{
	va_list args;
    va_start(args, log_id);
	
	printf("slog_log: %i \n", log_id&0x3);
	for(uint32_t i=0;i<(log_id&0x3);i++)
		printf("slog_log(%i): %i \n", i, va_arg(args, uint32_t));
}

int main()
{
	printf("... %i \n", GEN_LABEL(SLOGNAME, __LINE__));
	LOG_ERROR("", 501, 502, 503);
	return 0;
}
