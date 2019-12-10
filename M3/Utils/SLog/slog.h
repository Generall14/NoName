#ifndef __SLOG_H__
#define __SLOG_H__

#include <stdint.h>

/**
 * TODO: Python macros
 * TODO: description
 * 
 * TODO: buffer implementation (data and functions)
 * TODO: flushing implementation
 */

#define LEVEL_ERROR 0
#define LEVEL_WARNING 1
#define LEVEL_INFO 2
#define LEVEL_DEBUG 3

#define LOG_LEVEL LEVEL_DEBUG

// TODO implementation
#define LOG_ERROR
#define LOG_WARNING
#define LOG_INFO
#define LOG_DEBUG

#define SACOUNT_MASK 0x03

typedef struct
{
	uint32_t log_id; // look at slog_log_entry description
	uint32_t timestamp;
	uint32_t args[3]; // up to 3 arguments
} slog_entry;

/**
 * This function should not be called indirectly, 
 * only by LOG_XXX macro.
 * TODO: implementation
 * TODO: arguments descriptions
 * 
 * log_id: IIII IIII  IIII IIII  IIII IIII  IIII PPAA, where:
 * I - log id
 * P - log level (as in macros LEVEL_XXX)
 * A - argument count
 */
void slog_log_entry(uint32_t log_id, ...);

#endif
