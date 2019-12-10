#ifndef __SLOG_H__
#define __SLOG_H__

#include <stdint.h>

/**
 * TODO: Python macros
 * TODO: description
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

/**
 * This function should not be called indirectly, 
 * only by LOG_XXX macro.
 * TODO: implementation
 * TODO: arguments descriptions
 */
void slog_log_entry(uint32_t log_id, ...);

#endif
