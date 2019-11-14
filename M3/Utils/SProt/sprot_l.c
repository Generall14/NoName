#include "sprot_l.h"
#include "global_clock.h"

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

void process_fifo(sprot_fifo* fifo)
{
	//spr_timeout_head(fifo);
}

void spr_timeout_head(sprot_fifo* fifo)
{
	sprot_buff_entry* entry = get_spfifo_head(fifo);
	if(entry == 0)
		return;
	if(entry->status != SPROT_FILLING)
		return;
	if(get_global_clock_us() - entry->timestamp > SPROT_TIMEOUT)
		entry->status = SPROT_FULL;
}

sprot_buff_entry* get_spfifo_head(sprot_fifo* fifo)
{
	if(fifo->buffs[fifo->head].status == SPROT_EMPTY ||
	        fifo->buffs[fifo->head].status == SPROT_FILLING)
		return &(fifo->buffs[fifo->head]);

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

