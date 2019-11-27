#include "sprot_l.h"
#include "global_clock.h"
#include <string.h>

#define D_INITIAL_CRC8 0x55

uint8_t crc8t[] = {0x0, 0xcf, 0x51, 0x9e, 0xa2, 0x6d, 0xf3, 0x3c, 0x8b, 0x44, 0xda, 0x15, 0x29, 0xe6, 0x78, 0xb7, 0xd9,
		0x16, 0x88, 0x47, 0x7b, 0xb4, 0x2a, 0xe5, 0x52, 0x9d, 0x3, 0xcc, 0xf0, 0x3f, 0xa1, 0x6e, 0x7d, 0xb2, 0x2c, 0xe3,
		0xdf, 0x10, 0x8e, 0x41, 0xf6, 0x39, 0xa7, 0x68, 0x54, 0x9b, 0x5, 0xca, 0xa4, 0x6b, 0xf5, 0x3a, 0x6, 0xc9, 0x57,
		0x98, 0x2f, 0xe0, 0x7e, 0xb1, 0x8d, 0x42, 0xdc, 0x13, 0xfa, 0x35, 0xab, 0x64, 0x58, 0x97, 0x9, 0xc6, 0x71, 0xbe,
		0x20, 0xef, 0xd3, 0x1c, 0x82, 0x4d, 0x23, 0xec, 0x72, 0xbd, 0x81, 0x4e, 0xd0, 0x1f, 0xa8, 0x67, 0xf9, 0x36, 0xa,
		0xc5, 0x5b, 0x94, 0x87, 0x48, 0xd6, 0x19, 0x25, 0xea, 0x74, 0xbb, 0xc, 0xc3, 0x5d, 0x92, 0xae, 0x61, 0xff, 0x30,
		0x5e, 0x91, 0xf, 0xc0, 0xfc, 0x33, 0xad, 0x62, 0xd5, 0x1a, 0x84, 0x4b, 0x77, 0xb8, 0x26, 0xe9, 0x3b, 0xf4, 0x6a,
		0xa5, 0x99, 0x56, 0xc8, 0x7, 0xb0, 0x7f, 0xe1, 0x2e, 0x12, 0xdd, 0x43, 0x8c, 0xe2, 0x2d, 0xb3, 0x7c, 0x40, 0x8f,
		0x11, 0xde, 0x69, 0xa6, 0x38, 0xf7, 0xcb, 0x4, 0x9a, 0x55, 0x46, 0x89, 0x17, 0xd8, 0xe4, 0x2b, 0xb5, 0x7a, 0xcd,
		0x2, 0x9c, 0x53, 0x6f, 0xa0, 0x3e, 0xf1, 0x9f, 0x50, 0xce, 0x1, 0x3d, 0xf2, 0x6c, 0xa3, 0x14, 0xdb, 0x45, 0x8a,
		0xb6, 0x79, 0xe7, 0x28, 0xc1, 0xe, 0x90, 0x5f, 0x63, 0xac, 0x32, 0xfd, 0x4a, 0x85, 0x1b, 0xd4, 0xe8, 0x27, 0xb9,
		0x76, 0x18, 0xd7, 0x49, 0x86, 0xba, 0x75, 0xeb, 0x24, 0x93, 0x5c, 0xc2, 0xd, 0x31, 0xfe, 0x60, 0xaf, 0xbc, 0x73,
		0xed, 0x22, 0x1e, 0xd1, 0x4f, 0x80, 0x37, 0xf8, 0x66, 0xa9, 0x95, 0x5a, 0xc4, 0xb, 0x65, 0xaa, 0x34, 0xfb, 0xc7,
		0x8, 0x96, 0x59, 0xee, 0x21, 0xbf, 0x70, 0x4c, 0x83, 0x1d, 0xd2};

static uint8_t next_index(uint8_t current, sprot_fifo* fifo)
{
	current++;
	if(current >= SPROT_FIFO_ENTRIES)
		current = 0;
	return current;
}

void sprot_init_fifo(sprot_fifo* fifo)
{
	fifo->head = 0;
	fifo->tail = 0;
	for(int i=0; i<SPROT_FIFO_ENTRIES; i++)
		fifo->buffs[i].status = SPROT_EMPTY;
}

uint8_t calc_crc(uint8_t* buff, uint8_t bytes)
{
	uint8_t sum = D_INITIAL_CRC8;

    for(int i=0;i<bytes+1;++i) // For every byte and for flush data
    {
        uint8_t lval = 0x00;
        if(i<bytes)
            lval = buff[i]; // flush crc

        uint8_t arrPtr = sum&0xFF;
        sum = lval;
        sum ^= crc8t[arrPtr];
    }

    return sum&0xFF;
}

/**
 * True - command is ok.
 */
static bool is_buff_cmd_ok(sprot_buff_entry* buff)
{
	if((uint8_t)(buff->cmdHSize&0x7F) > 64)
		return false;

	if(buff->start != 0x5A)
		return false;

	if(buff->write_offseet != (uint8_t)((uint8_t)(buff->cmdHSize&0x7F) + 4))
		return false;

	// crc calculated with crc_byte - it should return zero if crc is correct
	uint8_t crc = calc_crc(&(buff->start), buff->write_offseet);
	if(crc)
		return false;

	return true;
}

static sprot_efunc* get_fun(sprot_efunc table[], uint16_t cmd, uint8_t tbl_entries)
{
	for(int i=0;i<tbl_entries;i++)
	{
		if(table[i].cmd==cmd)
			return &(table[i]);
	}
	return 0;
}

void process_fifo(sprot_fifo* fifo, sprot_efunc table[], void (*default_fun)(sprot_buff_entry*), uint16_t tbl_entries)
{
	sprot_buff_entry* entry = 0;
	while(1)
	{
		entry = get_spfifo_tail(fifo);

		if(!entry)
			return;
		entry->status = SPROT_EMPTY;

		if(!is_buff_cmd_ok(entry))
			continue;

		uint16_t cmd = ((entry->cmdHSize<<1)&0x100) | entry->cmdL;
		sprot_efunc* fun = get_fun(table, cmd, tbl_entries);
		if(!fun)
		{
			if(default_fun)
				(*default_fun)(entry);
			continue;
		}

		if(fun->size != 0xFF)
		{
			if(fun->size != (uint8_t)(entry->cmdHSize&0x7F))
				continue;
		}

		if(fun)
		{
			if(fun->fun_ptr)
				fun->fun_ptr(entry);
		}
	}
}

void spr_timeout_head(sprot_fifo* fifo)
{
	sprot_buff_entry* entry = get_spfifo_head(fifo);
	if(entry == 0)
		return;
	if(entry->status != SPROT_FILLING)
		return;
	if(get_global_clock_us() - entry->timestamp > SPROT_TIMEOUT_US)
		entry->status = SPROT_FULL;
}

sprot_buff_entry* get_spfifo_head(sprot_fifo* fifo)
{
	if(fifo->buffs[fifo->head].status == SPROT_EMPTY ||
	        fifo->buffs[fifo->head].status == SPROT_FILLING)
	{
		if(fifo->buffs[fifo->head].status == SPROT_EMPTY)
			fifo->buffs[fifo->head].write_offseet = 0;
		return &(fifo->buffs[fifo->head]);
	}

	uint8_t next = next_index(fifo->head, fifo);
	if(fifo->buffs[next].status == SPROT_EMPTY)
	{
		fifo->head = next;
		return &(fifo->buffs[next]);
	}
	else
		return 0;
}

sprot_buff_entry* get_spfifo_tail(sprot_fifo* fifo)
{
	if(fifo->buffs[fifo->tail].status == SPROT_FULL ||
	        fifo->buffs[fifo->tail].status == SPROT_PROCESSING)
		return &(fifo->buffs[fifo->tail]);

	uint8_t next = next_index(fifo->tail, fifo);
	if(fifo->buffs[next].status == SPROT_FULL)
	{
		fifo->tail = next;
		return &(fifo->buffs[next]);
	}
	else
		return 0;
}

uint8_t sp_push_bytes_to_fifo(sprot_fifo* fifo, uint8_t* buff, uint8_t bytes)
{
	spr_timeout_head(fifo);

	// Get next buffer
	sprot_buff_entry* head = get_spfifo_head(fifo);
	if(head == 0)
		return 0;

	// Update status and timestamp
	if(head->status==SPROT_EMPTY)
	{
		head->status = SPROT_FILLING;
		head->timestamp = get_global_clock_us();
	}

	// Copy data
	size_t bytes_to_cpy = SPROT_BUFF_CAPACITY-head->write_offseet;
	if(bytes<bytes_to_cpy)
		bytes_to_cpy = bytes;
	memcpy(&(head->start)+head->write_offseet, buff, bytes_to_cpy);
	head->write_offseet += bytes_to_cpy;

	// Finish buffer
	if(head->write_offseet == SPROT_BUFF_CAPACITY)
		head->status = SPROT_FULL;
	else if(head->write_offseet >= (head->cmdHSize&0x7F)+4)
		head->status = SPROT_FULL;

	return bytes_to_cpy;
}

uint8_t sp_push_command_to_fifo(sprot_fifo* fifo, uint16_t cmd, uint8_t* data, uint8_t bytes)
{
	// Get next buffer
	sprot_buff_entry* head = get_spfifo_head(fifo);
	if(head == 0)
		return 0;

	if(bytes>PACKAGE_DATA_BYTES)
		return 0;

	// Check and update status
	if(head->status==SPROT_EMPTY)
		head->status = SPROT_FULL;
	else
		return 0;
	head->write_offseet = bytes+4;

	// Copy data
	memcpy(&(head->data_and_crc), data, bytes);

	// Set other data
	head->start = 0x5A;
	head->cmdHSize = ((cmd>>1)&0x80) | (bytes&0x7F);
	head->cmdL = cmd&0xFF;
	head->data_and_crc[bytes] = calc_crc(&(head->start), bytes+3);

	return bytes+4;
}


