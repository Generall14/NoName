#ifndef _SPROT_L_H_
#define _SPROT_L_H_

/**
 * IMPORTANT: Those functions are not thread safe!
 * spr_timeout_head and sp_push_bytes_to_fifo have
 * to be called on the same interrupt level.
 *
 * USAGE:
 * 1. All fifo structures have to be initialized by sprot_init_fifo
 * 2. Call repetitively process_fifo and spr_timeout_head.
 * 3. Push new data into fifo by sp_push_bytes_to_fifo.
 *
 * TODO: better way to timeout commands?.
 * TODO: unit tests for process_fifo
 */

#include <stdint.h>
#include <stdbool.h>

#define SPROT_TIMEOUT_US 1000

#define PACKAGE_DATA_BYTES 64
#define SPROT_FIFO_ENTRIES 4
#define SPROT_BUFF_CAPACITY (PACKAGE_DATA_BYTES+4)

#define SPROT_EMPTY 0
#define SPROT_FULL 1
#define SPROT_FILLING 2
#define SPROT_PROCESSING 3

typedef struct
{
	uint8_t start;
	uint8_t cmdHSize;
	uint8_t cmdL;
	uint8_t data_and_crc[PACKAGE_DATA_BYTES+1];
	uint8_t status;
	uint8_t write_offseet;
	uint32_t timestamp;
} sprot_buff_entry;

typedef struct
{
	sprot_buff_entry buffs[SPROT_FIFO_ENTRIES];
	uint8_t tail; // buffer index
	uint8_t head; // buffer index
} sprot_fifo;

typedef struct
{
	uint8_t size; // 0xFF - Do not check before call
	uint16_t cmd; // command number
	void (*fun_ptr)(sprot_buff_entry*);
} sprot_efunc;

//======= User functions =========

void sprot_init_fifo(sprot_fifo* fifo);

/**
 * Must be called in loop to execute commands.
 * TODO: implementations.
 */
void process_fifo(sprot_fifo* fifo, sprot_efunc table[], void (*default_fun)(sprot_buff_entry*));

/**
 * Must be called in loop to timeout incoming commands.
 */
void spr_timeout_head(sprot_fifo* fifo);

/**
 * Pushes data into next available head buffer. When input buff
 * will be longer then available in head buff - input data will
 * be trimmed.
 * buff - buffer with data to be written.
 * bytes - bytes in buffer.
 * return - number of written bytes.
 * TODO: implementation
 */
uint8_t sp_push_bytes_to_fifo(sprot_fifo* fifo, uint8_t* buff, uint8_t bytes);

//================================

/**
 * Returns current head buffer from fifo (to fill with new
 * data), returns 0 when buffer is full. Updates pointer
 * when current head element have status SPROT_FULL and
 * next element is empty.
 */
sprot_buff_entry* get_spfifo_head(sprot_fifo* fifo);

/**
 * Returns current tail buffer from fifo (to read from),
 * returns 0 when buffer is empty. Updates pointer
 * when current tail element have status SPROT_EMPTY and
 * next element if full.
 */
sprot_buff_entry* get_spfifo_tail(sprot_fifo* fifo);

#endif
