#ifndef _SPROT_L_H_
#define _SPROT_L_H_

#include "stm32f1xx_hal.h"

#define SPROT_TIMEOUT 1000

#define PACKAGE_DATA_BYTES 64
#define SPROT_FIFO_ENTRIES 4

#define SPROT_EMPTY 0
#define SPROT_FULL 1
#define SPROT_FILLING 2
#define SPROT_PROCESSING 3

typedef struct
{
	uint8_t start;
	uint8_t cmdHsize;
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

void sprot_init_fifo(sprot_fifo* fifo);

/**
 * Must be called in loop to execute commands.
 */
void process_fifo(sprot_fifo* fifo);

/**
 * Must be called in loop timeout incoming commands.
 */
void spr_timeout_head(sprot_fifo* fifo);

//================================

//TODO:

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
