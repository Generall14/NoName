#ifndef __SLOG_H__
#define __SLOG_H__

#include <stdint.h>
#include <stdarg.h>
#include "utils.h"

/**
 * TODO: Python macros
 * TODO: description
 *
 * TODO: flushing implementation
 */

void slog_init();

#define LEVEL_NONE 0
#define LEVEL_ERROR 1
#define LEVEL_WARNING 2
#define LEVEL_INFO 3
#define LEVEL_DEBUG 4

#define LOG_LEVEL LEVEL_DEBUG

#define SACOUNT_MASK 0x03

#define GEN_LABEL(name, line) CAT3(name, _, line)
#define LOG_ENTRY(...) STUB(slog_log_entry(GEN_LABEL(SLOGNAME, __LINE__), ##__VA_ARGS__))

#if LOG_LEVEL >= LEVEL_ERROR
#define LOG_ERROR(TXT, ...) LOG_ENTRY(__VA_ARGS__)
#else
#define LOG_ERROR(TXT, ...)
#endif

#if LOG_LEVEL >= LEVEL_WARNING
#define LOG_WARNING(TXT, ...) LOG_ENTRY(__VA_ARGS__)
#else
#define LOG_WARNING(TXT, ...)
#endif

#if LOG_LEVEL >= LEVEL_INFO
#define LOG_INFO(TXT, ...) LOG_ENTRY(__VA_ARGS__)
#else
#define LOG_INFO(TXT, ...)
#endif

#if LOG_LEVEL >= LEVEL_DEBUG
#define LOG_DEBUG(TXT, ...) LOG_ENTRY(__VA_ARGS__)
#else
#define LOG_DEBUG(TXT, ...)
#endif

typedef struct
{
	uint32_t log_id; // look at slog_log_entry description
	uint32_t timestamp;
	uint32_t args[3]; // up to 3 arguments
} slog_entry;

#define SLOG_BUFF_WORDS 16
typedef struct
{
	uint8_t head; // offset in data to next free index
	uint8_t data_lost; // non zero value means entry cannot be created due to lack of memory
	uint32_t data[SLOG_BUFF_WORDS];
} slog_buff;
extern slog_buff slog_buffer;

/**
 * This function should not be called indirectly,
 * only by LOG_ERROR/WARNING/INFO/DEBUG macro.
 *
 * log_id: CCCI IIII  IIII IIII  IIII IIII  IIII PPAA, where:
 * I - log id
 * P - log level (as in macros LEVEL_XXX)
 * A - argument count
 * C - control sum, number of ones in log_id (excluding C)
 *
 * Up to 3 arguments uint32_t.
 */
void slog_log_entry(uint32_t log_id, ...);

void slog_push_entry(slog_entry *entry, slog_buff *buff);
void slog_clear_buff(slog_buff *buff);

#ifdef UTEST
void STUB_slog_log_entry(uint32_t log_id, ...);
void STUB_slog_push_entry(slog_entry *entry, slog_buff *buff);
#endif

#endif
