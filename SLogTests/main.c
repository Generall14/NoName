#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include "tst.h"

void slog_log(uint32_t log_id, ...)
{
	va_list args;
    va_start(args, log_id);
	
	printf("slog_log: %i \n", log_id);
	for(int i=0;i<log_id;i++)
		printf("slog_log(%i): %i \n", i, va_arg(args, uint32_t));
}

int main()
{
	int AA = 6;
	//LOG_ERROR(AA);
	printf("... %s \n", LOG_ERROR(hfg));
	printf("... %i \n", LOG_ERROR(hfg));
	printf("... %s \n", AT);
	printf("End\n");
	slog_log(1, 666);
	
	LOG_TEST(2, 102, 106);
	return 0;
}
