#include "header.h"

#include <SProt/sprot_l.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "fake/crc.h"

char invokes[6];
uint8_t* passed_ptr_dst;
uint8_t* passed_ptr_src;
uint8_t passed_bytes;
sprot_fifo ofifo;
sprot_buff_entry ibuff;

#define FRWS_R_OFF 0
#define FRWS_W_OFF 1
#define SRWS_BYTES 10
#define SRWS_NUM 22

#define FRWL_R_OFF 2
#define FRWL_W_OFF 3
#define SRWL_BYTES 100
#define SRWL_NUM 44

#define FRS_R_OFF 4
#define SRS_BYTES 11
#define SRS_NUM 23

#define FRL_R_OFF 5
#define SRL_BYTES 101
#define SRL_NUM 0

uint8_t SRWS[SRWS_BYTES];
uint8_t SRWL[SRWL_BYTES];
uint8_t SRS[SRS_BYTES];
uint8_t SRL[SRL_BYTES];

void FRWS_R(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRWS_R_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

void FRWS_W(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRWS_W_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

void FRWL_R(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRWL_R_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

void FRWL_W(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRWL_W_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

void FRS_R(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRS_R_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

void FRL_R(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	invokes[FRL_R_OFF]++;
	passed_ptr_dst = dest;
	passed_ptr_src = src;
	passed_bytes = bytes;
}

sprot_section stbl[] = \
	{\
		{.number=SRWS_NUM, .data_ptr=SRWS, .bytes=SRWS_BYTES, .fun_read_cpy=FRWS_R, .fun_write_cpy=FRWS_W},\
		{.number=SRWL_NUM, .data_ptr=SRWL, .bytes=SRWL_BYTES, .fun_read_cpy=FRWL_R, .fun_write_cpy=FRWL_W},\
		{.number=SRS_NUM, .data_ptr=SRS, .bytes=SRS_BYTES, .fun_read_cpy=FRS_R, .fun_write_cpy=0},\
		{.number=SRL_NUM, .data_ptr=SRL, .bytes=SRL_BYTES, .fun_read_cpy=FRL_R, .fun_write_cpy=0}\
	};

void prepare_read_cmd(uint8_t number, uint16_t offset)
{
	ibuff.data_and_crc[0] = number;
	ibuff.data_and_crc[1] = offset&0xFF;
	ibuff.data_and_crc[2] = (offset>>8)&0xFF;
}

void prepare_write_cmd(uint8_t number, uint16_t offset, uint8_t bytes)
{
	ibuff.data_and_crc[0] = number;
	ibuff.data_and_crc[1] = offset&0xFF;
	ibuff.data_and_crc[2] = (offset>>8)&0xFF;
	ibuff.cmdHSize = (bytes+4)&0x7F;
}

// retrurn true when command is ok
bool validate_cmd_reread(uint8_t number, uint16_t offset, uint8_t bytes)
{
	if(ofifo.buffs[0].start != 0x5A)
		return false;
	if(ofifo.buffs[0].cmdHSize != (bytes+4+3)&0x7F)
		return false;
	if(ofifo.buffs[0].cmdL != 0x01)
		return false;
	if(ofifo.buffs[0].status != SPROT_FULL)
		return false;
	if(ofifo.buffs[0].data_and_crc[0] != number)
		return false;
	if(ofifo.buffs[0].data_and_crc[1] != offset&0xFF)
		return false;
	if(ofifo.buffs[0].data_and_crc[2] != (offset>>8)&0xFF)
		return false;
	if(crc(&(ofifo.buffs[0].start), bytes+4))
		return false;
	return true;
}

bool validate_cmd_rewrite(uint8_t number, uint16_t offset, uint8_t status)
{
	if(ofifo.buffs[0].start != 0x5A)
		return false;
	if(ofifo.buffs[0].cmdHSize != (4+4)&0x7F)
		return false;
	if(ofifo.buffs[0].cmdL != 0x01)
		return false;
	if(ofifo.buffs[0].status != SPROT_FULL)
		return false;
	if(ofifo.buffs[0].data_and_crc[0] != number)
		return false;
	if(ofifo.buffs[0].data_and_crc[1] != offset&0xFF)
		return false;
	if(ofifo.buffs[0].data_and_crc[2] != (offset>>8)&0xFF)
		return false;
	if(ofifo.buffs[0].data_and_crc[3] != status)
		return false;
	if(crc(&(ofifo.buffs[0].start), 4+4))
		return false;
	return true;
}

// retrurn true when parameters is ok
bool validate_passed_parameters(uint8_t* ptr_dst, uint8_t* ptr_src, uint8_t bytes)
{
	if(ptr_dst != passed_ptr_dst)
		return false;
	if(ptr_src != passed_ptr_src)
		return false;
	if(passed_bytes != bytes)
		return false;
	return true;
}




void test_sprot_read_success()
{
	// Valid short section - start
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={1, 0, 0, 0, 0, 0};
	prepare_read_cmd(SRWS_NUM, 0);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(SRWS, &(ofifo.buffs[0].data_and_crc[3]), SRWS_BYTES), \
	"Valid short section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWS_NUM, 0, SRWS_BYTES), \
	"Valid short section - start - output buffer should be prepered - start, cmd, size, crc");

	// Valid short section - mid
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={1, 0, 0, 0, 0, 0};
	prepare_read_cmd(SRWS_NUM, 2);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - mid - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(SRWS[2]), &(ofifo.buffs[0].data_and_crc[3]), SRWS_BYTES-2), \
	"Valid short section - mid - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWS_NUM, 2, SRWS_BYTES-2), \
	"Valid short section - mid - output buffer should be prepered - start, cmd, size, crc");
	
	// Valid long section - start
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 0, 1, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM, 0);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(SRWL[0]), &(ofifo.buffs[0].data_and_crc[3]), 60), \
	"Valid long section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, 0, 60), \
	"Valid long section - start - output buffer should be prepered - start, cmd, size, crc");
	
	// Valid long section - mid
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 1, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM, SRWL_BYTES-60);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - mid - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(SRWL[SRWL_BYTES-60]), &(ofifo.buffs[0].data_and_crc[3]), 60), \
	"Valid long section - mid - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, SRWL_BYTES-60, 60), \
	"Valid long section - mid - output buffer should be prepered - start, cmd, size, crc");
	
	// Valid long section - end
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected5[]={0, 0, 1, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM, SRWL_BYTES-30);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - end - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(SRWL[SRWL_BYTES-30]), &(ofifo.buffs[0].data_and_crc[3]), 30), \
	"Valid long section - end - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, SRWL_BYTES-30, 30), \
	"Valid long section - end - output buffer should be prepered - start, cmd, size, crc");
}

void test_sprot_read_fail()
{
	memset(&ofifo, 0x00, sizeof(ofifo));
	
	// Valid short section - offset out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc
	
	// Valid long section - offset out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc
	
	// Non existing section
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc
}

void test_sprot_write_success()
{
	memset(&ofifo, 0x00, sizeof(ofifo));
	
	// Valid short section - start
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid short section - mid
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - start
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - mid
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - end
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid short section - data size shorter than section
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - data size shorter than section
	// Proper functions should be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
}

void test_sprot_write_fail()
{
	memset(&ofifo, 0x00, sizeof(ofifo));
	
	// Valid short section - offset out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - offset out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Non existing section
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid short section - size out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - size out of range
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid short section - read only
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
	
	// Valid long section - read only
	// Proper functions should not be called
	// Function parameters should be valid
	// Output buffer should be prepered - start, cmd, size, crc, status
}

// 	// Valid command with long cmd
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected1[]={1, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid command with long cmd - proper command should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Valid command with long cmd - buff should be changed to SPROT_EMPTY");
// 	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
// 	"Valid command with long cmd - proper pointer should be passed");
// 
// 	// Valid command with variable size, pt1
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected2[]={0, 1, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 1, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid command with variable size, pt1 - proper command should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Valid command with variable size, pt1 - buff should be changed to SPROT_EMPTY");
// 	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
// 	"Valid command with variable size, pt1 - proper pointer should be passed");
// 
// 	// Valid command with variable size, pt2
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected3[]={0, 1, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 10, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid command with variable size, pt2 - proper command should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Valid command with variable size, pt2 - buff should be changed to SPROT_EMPTY");
// 	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
// 	"Valid command with variable size, pt2 - proper pointer should be passed");
// 	
// 	// Valid command with no data
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected4[]={0, 0, 1, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F03_CMD, F03_SIZE, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid command with no data - proper command should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Valid command with no data - buff should be changed to SPROT_EMPTY");
// 	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
// 	"Valid command with no data - proper pointer should be passed");
// 
// 	// Valid but unknown command
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected5[]={0, 0, 0, 1};
// 	prepare_cmd(&(fifo.buffs[0]), 0xAA, 5, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid but unknown command - default command should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Valid but unknown command - buff should be changed to SPROT_EMPTY");
// 	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
// 	"Valid but unknown command - proper pointer should be passed");
// }
// 
// void test_spexe_invalid_commands()
// {
// 	sprot_fifo fifo;
// 	
// 	// Invalid crc
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected1[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, false, true);
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Invalid crc - no commands should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Invalid crc - buff should be changed to SPROT_EMPTY");
// 	
// 	// Invalid start byte
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected2[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, true, false);
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Invalid start byte - no commands should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Invalid start byte - buff should be changed to SPROT_EMPTY");
// 	
// 	// Invalid offset
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected3[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, true, false, false);
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Invalid offset - no commands should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Invalid offset - buff should be changed to SPROT_EMPTY");
// 	
// 	// Invalid command size
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected4[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE+1, false, false, false);
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Invalid command size - no commands should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Invalid command size - buff should be changed to SPROT_EMPTY");
// 	
// 	// Known command with variable size, cmd_size bigger than allowed
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected5[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 65, false, false, false);
// 	process_fifo(&fifo, 0, tbl, fdef, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Known command with variable size, cmd_size bigger than allowed - no "
// 	"commands should be executed");
// 	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
// 	"Known command with variable size, cmd_size bigger than allowed - buff "
// 	"should be changed to SPROT_EMPTY");
// }
// 
// void test_spexe_null_ptrs()
// {
// 	sprot_fifo fifo;
// 	
// 	// Valid command - empty pointer to function
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected1[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), FNULL_CMD, FNULL_SIZE, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, 0, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid command - empty pointer to function - no commands should be executed");
// 	
// 	// Valid but unknown command - empty pointer to function
// 	memset(&fifo, 0x00, sizeof(fifo));
// 	fifo.buffs[0].status = SPROT_FULL;
// 	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
// 	char expected2[]={0, 0, 0, 0};
// 	prepare_cmd(&(fifo.buffs[0]), 0xAA, 5, false, false, false);
// 	passed_ptr = 0;
// 	process_fifo(&fifo, 0, tbl, 0, ENTR);
// 	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
// 	"Valid but unknown command - empty pointer to function - no commands should be executed");
// }
// 
// void test_spexe_crc()
// {
// 	uint8_t tab1[] = {0x11, 0x00, 0x57, 0x91, 0x00};
// 	uint8_t s = sizeof(tab1)/sizeof(tab1[0]);
// 	TEST_CHECK_P(crc(tab1, s)==calc_crc(tab1, s), \
// 	"calc crc, data set 1");
// 	
// 	uint8_t tab2[] = {0x11, 0x00, 0x57, 0x91, 0x00};
// 	s = sizeof(tab2)/sizeof(tab2[0]);
// 	TEST_CHECK_P(crc(tab2, s)==calc_crc(tab2, s), \
// 	"calc crc, data set 1");
// 	
// 	uint8_t tab3[] = {0x11, 0x00, 0x57, 0x91, 0x00};
// 	s = sizeof(tab3)/sizeof(tab3[0]);
// 	TEST_CHECK_P(crc(tab3, s)==calc_crc(tab3, s), \
// 	"calc crc, data set 1");
// 	
// 	uint8_t tab4[] = {0x11, 0x00, 0x57, 0x91, 0x00};
// 	s = sizeof(tab4)/sizeof(tab4[0]);
// 	TEST_CHECK_P(crc(tab4, s)==calc_crc(tab4, s), \
// 	"calc crc, data set 1");
// }
// 
