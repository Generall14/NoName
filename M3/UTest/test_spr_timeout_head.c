#include "header.h"

#include <SProt/sprot_l.h>

extern uint32_t fake_global_clock_us;

void test_init_fifo()
{
	sprot_fifo fifo;
	memset(&fifo, 0xff, sizeof(fifo));
	sprot_init_fifo(&fifo);
	
	TEST_CHECK_P(fifo.head==0, "pc_fifo.head: %d", fifo.head);
	TEST_CHECK_P(fifo.tail==0, "pc_fifo.tail: %d", fifo.tail);
	
	for(int entry=0;entry<SPROT_FIFO_ENTRIES;entry++)
	{
		TEST_CHECK_P(fifo.buffs[entry].status==SPROT_EMPTY, \
		"fifo.buffs[%d].status: %d", entry, fifo.buffs[entry].status);
	}
}

void test_get_head_fifo()
{
	sprot_fifo fifo;
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	sprot_buff_entry* head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[0], \
	"Fifo is empty - should return firs buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[0], \
	"First buff is SPROT_FILLING - should return firs buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[1], \
	"First buff is SPROT_FULL - should return second buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_PROCESSING;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[1], \
	"First buff is SPROT_PROCESSING - should return second buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	fifo.buffs[1].status = SPROT_FULL;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==0, \
	"First and second buff is SPROT_FULL - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	fifo.buffs[1].status = SPROT_PROCESSING;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==0, \
	"First buff is SPROT_FULL, second is SPROT_PROCESSING - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	fifo.buffs[1].status = SPROT_FILLING;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==0, \
	"First buff is SPROT_FULL, second is SPROT_FILLING - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.head = SPROT_FIFO_ENTRIES-1;
	fifo.buffs[SPROT_FIFO_ENTRIES-1].status = SPROT_FULL;
	head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[0], \
	"Head points to last buff, last buff is SPROT_FULL - should roll back and "
	"return firs buff");
}

void test_get_tail_fifo()
{
	sprot_fifo fifo;
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	sprot_buff_entry* tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==&fifo.buffs[0], \
	"First is SPROT_FULL - should return firs buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_PROCESSING;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==&fifo.buffs[0], \
	"First is SPROT_PROCESSING - should return firs buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[1].status = SPROT_FULL;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==&fifo.buffs[1], \
	"First is SPROT_EMPTY, second is SPROT_FULL - should return second buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[1].status = SPROT_FULL;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==&fifo.buffs[1], \
	"First is SPROT_FILLING, second is SPROT_FULL - should return second buff");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[1].status = SPROT_EMPTY;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==0, \
	"First is SPROT_EMPTY, second is SPROT_EMPTY - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[1].status = SPROT_FILLING;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==0, \
	"First is SPROT_FILLING, second is SPROT_EMPTY - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[1].status = SPROT_PROCESSING;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==0, \
	"First is SPROT_PROCESSING, second is SPROT_EMPTY - should return zero");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.head = SPROT_FIFO_ENTRIES-1;
	fifo.buffs[SPROT_FIFO_ENTRIES-1].status = SPROT_EMPTY;
	fifo.buffs[0].status = SPROT_FULL;
	tail = get_spfifo_tail(&fifo);
	TEST_CHECK_P(tail==&fifo.buffs[0], \
	"Head points to last buff, last buff is SPROT_EMPTY, first is SPROT_FULL "
	"- should roll back and return firs buff");
}

void test_timeout_head()
{
	const uint32_t timestart = 666;
	const uint32_t timeouted = timestart+SPROT_TIMEOUT_US+1;
	const uint32_t nottimeouted = timestart+SPROT_TIMEOUT_US-1;
	sprot_fifo fifo;
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[0].timestamp = timestart;
	fake_global_clock_us = nottimeouted;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FILLING, \
	"First is SPROT_FILLING and not timeouted - status should be SPROT_FILLING");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[0].timestamp = timestart;
	fake_global_clock_us = timeouted;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"First is SPROT_FILLING and timeouted - status should be SPROT_FULL");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[0].timestamp = timestart;
	fake_global_clock_us = timeouted;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"First is SPROT_EMPTY and timeouted - status should be SPROT_EMPTY");
}
