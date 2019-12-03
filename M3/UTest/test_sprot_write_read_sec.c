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

#define STATUS_OK 0x00
#define STATUS_RO 0x01
#define STATUS_OOR 0x02
#define STATUS_US 0x03

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
	TEST_CHECK_P(validate_passed_parameters(&(SRWL[0]), &(ofifo.buffs[0].data_and_crc[3]), 61), \
	"Valid long section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, 0, 61), \
	"Valid long section - start - output buffer should be prepered - start, cmd, size, crc");
	
	// Valid long section - mid
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 1, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM, SRWL_BYTES-61);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - mid - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(SRWL[SRWL_BYTES-61]), &(ofifo.buffs[0].data_and_crc[3]), 61), \
	"Valid long section - mid - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, SRWL_BYTES-61, 61), \
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
	// Valid short section - offset out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={0, 0, 0, 0, 0, 0};
	prepare_read_cmd(SRWS_NUM, SRWS_BYTES+1);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - offset out of range - functions should not be called");
	TEST_CHECK_P(validate_cmd_reread(SRWS_NUM, SRWS_BYTES+1, 0), \
	"Valid short section - offset out of range - output buffer should be prepered");
	
	// Valid long section - offset out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 0, 0, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM, SRWL_BYTES+1);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - offset out of range - functions should not be called");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM, SRWL_BYTES+1, 0), \
	"Valid short section - offset out of range - output buffer should be prepered");
	
	// Non existing section
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 0, 0, 0, 0, 0};
	prepare_read_cmd(SRWL_NUM+1, SRWL_BYTES+1);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - offset out of range - functions should not be called");
	TEST_CHECK_P(validate_cmd_reread(SRWL_NUM+1, SRWL_BYTES+1, 0), \
	"Valid short section - offset out of range - output buffer should be prepered");
}

void test_sprot_write_success()
{
	// Valid short section - start
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={0, 1, 0, 0, 0, 0};
	prepare_write_cmd(SRWS_NUM, 0, SRWS_BYTES);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(ofifo.buffs[0].data_and_crc[3]), &(SRWS[0]), SRWS_BYTES), \
	"Valid short section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_rewrite(SRWS_NUM, 0, STATUS_OK), \
	"Valid short section - start - output buffer should be prepered");
	
	// Valid short section - mid
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 1, 0, 0, 0, 0};
	prepare_write_cmd(SRWS_NUM, 1, SRWS_BYTES-1);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - mid - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(ofifo.buffs[0].data_and_crc[3]), &(SRWS[1]), SRWS_BYTES-1), \
	"Valid short section - mid - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_rewrite(SRWS_NUM, 1, STATUS_OK), \
	"Valid short section - mid - output buffer should be prepered");
	
	// Valid long section - start
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 0, 0, 1, 0, 0};
	prepare_write_cmd(SRWL_NUM, 0, 10);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(ofifo.buffs[0].data_and_crc[3]), &(SRWL[0]), 10), \
	"Valid long section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM, 0, STATUS_OK), \
	"Valid long section - start - output buffer should be prepered");
	
	// Valid long section - mid
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 0, 1, 0, 0};
	prepare_write_cmd(SRWL_NUM, 1, 10);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(ofifo.buffs[0].data_and_crc[3]), &(SRWL[1]), 10), \
	"Valid long section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM, 1, STATUS_OK), \
	"Valid long section - start - output buffer should be prepered");
	
	// Valid long section - end
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected5[]={0, 0, 0, 1, 0, 0};
	prepare_write_cmd(SRWL_NUM, SRWL_BYTES-10, 10);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - start - proper functions should be called");
	TEST_CHECK_P(validate_passed_parameters(&(ofifo.buffs[0].data_and_crc[3]), &(SRWL[SRWL_BYTES-10]), 10), \
	"Valid long section - start - function parameters should be valid");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM, SRWL_BYTES-10, STATUS_OK), \
	"Valid long section - start - output buffer should be prepered");
}

void test_sprot_write_fail()
{
	// Valid short section - offset out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRWS_NUM, SRWS_BYTES+2, 2);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - offset out of range - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRWS_NUM, SRWS_BYTES+2, STATUS_OOR), \
	"Valid short section - offset out of range - output buffer should be prepered");
	
	// Valid long section - offset out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRWL_NUM, SRWL_BYTES+2, 2);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - offset out of range - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM, SRWL_BYTES+2, STATUS_OOR), \
	"Valid long section - offset out of range - output buffer should be prepered");
	
	// Non existing section
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRWL_NUM+1, SRWL_BYTES+2, 2);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Non existing section - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM+1, SRWL_BYTES+2, STATUS_US), \
	"Non existing section - output buffer should be prepered - start, cmd, size, crc");
	
	// Valid short section - size out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRWS_NUM, 0, SRWS_BYTES);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - size out of range - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRWS_NUM, 0, STATUS_OOR), \
	"Valid short section - size out of range - output buffer should be prepered");
	
	// Valid long section - size out of range
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected5[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRWL_NUM, SRWL_BYTES-2, 5);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - offset out of range - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRWL_NUM, SRWL_BYTES-2, STATUS_OOR), \
	"Valid long section - offset out of range - output buffer should be prepered");
	
	// Valid short section - read only
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected6[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRS_NUM, 0, SRS_BYTES);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected6, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid short section - read only - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRS_NUM, 0, STATUS_RO), \
	"Valid short section - read only - output buffer should be prepered");
	
	// Valid long section - read only
	memset(&ofifo, 0x00, sizeof(ofifo));
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected7[]={0, 0, 0, 0, 0, 0};
	prepare_write_cmd(SRL_NUM, SRL_BYTES-60, SRL_BYTES);
	sprot_read_sec(&ibuff, &ofifo);
	TEST_CHECK_P(memcmp((void*)expected7, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid long section - read only - proper functions should not be called");
	TEST_CHECK_P(validate_cmd_rewrite(SRL_NUM, SRL_BYTES-60, STATUS_RO), \
	"Valid long section - read only - output buffer should be prepered");
}
