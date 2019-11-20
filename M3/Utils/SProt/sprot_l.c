#include "sprot_l.h"
#include "global_clock.h"
#include <string.h>

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

void process_fifo(sprot_fifo* fifo, sprot_efunc table[], void (*default_fun)(sprot_buff_entry*))
{

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

