#include "header.h"

#include <SProt/sprot_l.h>
#include <stdbool.h>
#include <string.h>
#include "fake/crc.h"

extern uint32_t fake_global_clock_us;
#define TIMESTART 1
#define TIMEOUTED (TIMESTART+SPROT_TIMEOUT_US+1)
#define NOTTIMEOUTED (TIMESTART+SPROT_TIMEOUT_US-1)
#define IBUFF_SIZE PACKAGE_DATA_BYTES*2

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
	fifo.buffs[0].write_offseet = 0xff;
	sprot_buff_entry* head = get_spfifo_head(&fifo);
	TEST_CHECK_P(head==&fifo.buffs[0], \
	"Fifo is empty - should return firs buff");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==0, \
	"Fifo is empty - head write_offseet should be zeroed");
	
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
	sprot_fifo fifo;
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[0].timestamp = TIMESTART;
	fake_global_clock_us = NOTTIMEOUTED;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FILLING, \
	"First is SPROT_FILLING and not timeouted - status should be SPROT_FILLING");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[0].timestamp = TIMESTART;
	fake_global_clock_us = TIMEOUTED;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"First is SPROT_FILLING and timeouted - status should be SPROT_FULL");
	
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_EMPTY;
	fifo.buffs[0].timestamp = TIMESTART;
	fake_global_clock_us = TIMEOUTED;
	spr_timeout_head(&fifo);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"First is SPROT_EMPTY and timeouted - status should be SPROT_EMPTY");
}

void prepare_idata(uint8_t ibuff[])
{
	for(int i=0;i<IBUFF_SIZE;i++)
		ibuff[i] = (i+20)&0xFF;
}

bool validate_idata(uint8_t ibuff[])
{
	for(int i=0;i<IBUFF_SIZE;i++)
	{
		if(ibuff[i] != (i+20)&0xFF)
			return false;
	}
	return true;
}

void test_push_fifo()
{
	sprot_fifo fifo;
	uint8_t ibuff[IBUFF_SIZE];
	uint8_t writted;
	
	// First write to empty buff
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_EMPTY;
	fake_global_clock_us = NOTTIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 2);
	TEST_CHECK_P(fifo.buffs[0].timestamp==NOTTIMEOUTED, \
	"Write to SPROT_EMPTY buff - timestamp should be updated");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FILLING, \
	"Write to SPROT_EMPTY buff - buff should be changed to SPROT_FILLING");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==2, \
	"Write to SPROT_EMPTY buff - write offset should be updated");
	TEST_CHECK_P(validate_idata(ibuff), \
	"Write to SPROT_EMPTY buff - input data should not be changed");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[0].start), 2)==0, \
	"Write to SPROT_EMPTY buff - data should be coppied");
	TEST_CHECK_P(writted==2, \
	"Write to SPROT_EMPTY buff - should return ibuff size");
	
	// Next write to not timeouted buff
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FILLING;
	fake_global_clock_us = NOTTIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 2);
	TEST_CHECK_P(fifo.buffs[0].timestamp==0, \
	"Write to SPROT_FILLING buff - timestamp should not be updated");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FILLING, \
	"Write to SPROT_EMPTY buff - buff should not be changed");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==2, \
	"Write to SPROT_FILLING buff - write offset should be updated");
	TEST_CHECK_P(validate_idata(ibuff), \
	"Write to SPROT_FILLING buff - input data should not be changed");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[0].start), 2)==0, \
	"Write to SPROT_FILLING buff - data should be coppied");
	TEST_CHECK_P(writted==2, \
	"Write to SPROT_FILLING buff - should return ibuff size");
	
	// Next write to timeouted buff
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FILLING;
	fake_global_clock_us = TIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 2);
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"Write to SPROT_FILLING buff, after timeouted - buff should be changed to SPROT_FULL");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==0, \
	"Write to SPROT_FILLING buff - write offset should not be updated");
	TEST_CHECK_P(validate_idata(ibuff), \
	"Write to SPROT_FILLING buff - input data should not be changed");
	TEST_CHECK_P(fifo.buffs[1].status==SPROT_FILLING, \
	"Write to SPROT_FILLING buff, after timeouted - next buff should be used and changed"
	" to SPROT_FILLING");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[1].start), 2)==0, \
	"Write to SPROT_FILLING buff, after timeouted - next buff should be used and writed"
	" data to it");
	TEST_CHECK_P(writted==2, \
	"Write to SPROT_FILLING buff, after timeouted - should return ibuff size");
	
	// Write with non zero offset
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FILLING;
	fifo.buffs[0].write_offseet = 4;
	fake_global_clock_us = NOTTIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 10);
	TEST_CHECK_P(writted==10, \
	"Write with non zero offset - should return ibuff size");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==14, \
	"Write with non zero offset - write offset should be updated");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[0].start)+4, 10)==0, \
	"Write with non zero offset - data should be coppied");
	TEST_CHECK_P(validate_idata(ibuff), \
	"Write with non zero offset - input data should not be changed");
	
	// ibuff bigger than buffer available size
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FILLING;
	fake_global_clock_us = NOTTIMEOUTED;
	fifo.buffs[0].write_offseet = 1;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, IBUFF_SIZE);
	TEST_CHECK_P(writted==PACKAGE_DATA_BYTES+4-1, \
	"ibuff bigger than buffer available size - should return available size");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"ibuff bigger than buffer available size - buff should be changed to SPROT_FULL");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[0].start)+1, PACKAGE_DATA_BYTES+4-1)==0, \
	"ibuff bigger than buffer available size - data should be coppied");
	TEST_CHECK_P(validate_idata(ibuff), \
	"ibuff bigger than buffer available size - input data should not be changed");
	
	// Full command recieved
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	ibuff[1]=10&0x7F; // cmd:bytes
	fifo.buffs[0].status = SPROT_FILLING;
	fake_global_clock_us = NOTTIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 10+4);
	TEST_CHECK_P(writted==10+4, \
	"Full command recieved - should return ibuff size");
	TEST_CHECK_P(memcmp((void*)ibuff, (void*)&(fifo.buffs[0].start), 10+4)==0, \
	"Full command recieved - data should be coppied");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"Full command recieved - buff should be changed to SPROT_FULL");
	
	// No buffers available
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FULL;
	fifo.buffs[1].status = SPROT_FULL;
	fake_global_clock_us = NOTTIMEOUTED;
	writted = sp_push_bytes_to_fifo(&fifo, ibuff, 10);
	TEST_CHECK_P(writted==0, \
	"No buffers available - should return zero");
}

void test_push_command()
{
	sprot_fifo fifo;
	uint8_t ibuff[IBUFF_SIZE];
	uint8_t writted;
	
	// No buffers available
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_FULL;
	fifo.buffs[1].status = SPROT_FULL;
	writted = sp_push_command_to_fifo(&fifo, 1, ibuff, 10);
	TEST_CHECK_P(writted==0, \
	"No buffers available - should return zero");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==0, \
	"No buffers available - write offset should not be updated");
	
	// ibuff bigger than buffer available size
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_EMPTY;
	writted = sp_push_command_to_fifo(&fifo, 1, ibuff, IBUFF_SIZE);
	TEST_CHECK_P(writted==0, \
	"ibuff bigger than buffer available size - should return zero");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"ibuff bigger than buffer available size - buff should not be changed");
	TEST_CHECK_P(validate_idata(ibuff), \
	"ibuff bigger than buffer available size - input data should not be changed");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==0, \
	"ibuff bigger than buffer available size - write offset should not be updated");
	
	// command without payload
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_EMPTY;
	uint8_t exp1[] = {0x5a, 0x80, 0x11};
	writted = sp_push_command_to_fifo(&fifo, 0x111, ibuff, 0);
	TEST_CHECK_P(writted==4, \
	"command without payload - should return full package size");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"command without payload - buff should be changed to SPROT_FULL");
	TEST_CHECK_P(validate_idata(ibuff), \
	"command without payload - input data should not be changed");
	TEST_CHECK_P(memcmp((void*)exp1, (void*)&(fifo.buffs[0].start), 3)==0, \
	"command without payload - proper data should be in fifo");
	TEST_CHECK_P(crc((void*)&(fifo.buffs[0].start), 4)==0, \
	"command without payload - valid crc should be calculated");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==4, \
	"command without payload - write offset should be updated");
	
	// command with payload
	memset(&fifo, 0x00, sizeof(fifo));
	prepare_idata(ibuff);
	fifo.buffs[0].status = SPROT_EMPTY;
	uint8_t exp2[] = {0x5a, 0x82, 0x02, 20, 21};
	writted = sp_push_command_to_fifo(&fifo, 0x102, ibuff, 2);
	TEST_CHECK_P(writted==6, \
	"command with payload - should return full package size");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_FULL, \
	"command with payload - buff should be changed to SPROT_FULL");
	TEST_CHECK_P(validate_idata(ibuff), \
	"command with payload - input data should not be changed");
	TEST_CHECK_P(memcmp((void*)exp2, (void*)&(fifo.buffs[0].start), 5)==0, \
	"command with payload - proper data should be in fifo");
	TEST_CHECK_P(crc((void*)&(fifo.buffs[0].start), 6)==0, \
	"command with payload - valid crc should be calculated");
	TEST_CHECK_P(fifo.buffs[0].write_offseet==6, \
	"command with payload - write offset should be updated");
}
