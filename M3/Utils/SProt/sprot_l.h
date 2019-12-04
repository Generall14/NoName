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
 * 4. Push full command (to output fifo) by sp_push_command_to_fifo.
 *
 * Receive fifo:
 * media -> sp_push_bytes_to_fifo
 * loop -> spr_timeout_head
 * loop -> process_fifo -> exec input data by functions
 *
 * Transmit fifo:
 * somewhere in code -> sp_push_command_to_fifo
 * loop -> process_fifo -> send to media by function
 *
 * TODO: better way to timeout commands?.
 *
 *
 * Data sections:
 * Every data section have to be described in sprot_section structure
 * (all in one table). Functions fun_read_cpy/fun_write_cpy are
 * intended to copy data in read/write, if function pointer will
 * be NULL the transaction will not be allowed (read only if
 * fun_write_cpy will be zero).
 *
 * Functions sprot_write_sec / sprot_read_sec have to be pointed
 * in commands table (sprot_efunc spt_sec_tbl[]) with GetSec /
 * SetSec commands.
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

#define SPROT_START 0x5A

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
	void (*fun_ptr)(sprot_buff_entry*, sprot_fifo*);
} sprot_efunc;

/**
 * Describes data section.
 * fun_read_cpy/fun_write_cpy - pointers to functions copying
 * data on read / write. Those functions will be called after
 * validating command input data.
 */
typedef struct
{
	uint8_t number; // Section number
	uint8_t* data_ptr; // Pointer to data section
	uint16_t bytes; // Bytes in data section
	void (*fun_read_cpy)(uint8_t* dest, uint8_t* src, uint8_t bytes);
	void (*fun_write_cpy)(uint8_t* dest, uint8_t* src, uint8_t bytes);
} sprot_section; // TODO: some lockers / timestamps?

extern sprot_section spt_sec_tbl[]; // Have to be defined in user implementation
extern uint8_t spt_sec_tbl_entries; // Have to be defined in user implementation

//======= User functions =========

void sprot_init_fifo(sprot_fifo* fifo);

/**
 * Functions handling GetSec / SetSec. Validating input command
 * parameters, preparing response buffer,  calling copy functions.
 */
void sprot_write_sec(sprot_buff_entry* buff, sprot_fifo* re_fifo);
void sprot_read_sec(sprot_buff_entry* buff, sprot_fifo* re_fifo);

/**
 * Must be called in loop to execute commands.
 * fifo - executing fifo
 * re_fifo - fifo used to send response, can be 0 when output fifo is executing
 * table - table with commands
 * default_fun - default function (used to commands not included in table)
 * tbl_entries - entries in table
 */
void process_fifo(sprot_fifo* fifo, sprot_fifo* re_fifo, sprot_efunc table[],
		void (*default_fun)(sprot_buff_entry*, sprot_fifo*), uint16_t tbl_entries);

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
 */
uint8_t sp_push_bytes_to_fifo(sprot_fifo* fifo, uint8_t* buff, uint8_t bytes);

/**
 * Pushes full command into fifo (instantly changes state to SPROT_FULL, ignoring
 * timeouts).
 */
uint8_t sp_push_command_to_fifo(sprot_fifo* fifo, uint16_t cmd, uint8_t* data, uint8_t bytes);

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

uint8_t calc_crc(uint8_t* buff, uint8_t bytes);

#endif
