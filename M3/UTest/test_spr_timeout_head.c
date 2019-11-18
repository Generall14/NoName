#include "header.h"

#include <SProt/sprot_l.h>

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
