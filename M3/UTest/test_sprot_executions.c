#include "header.h"

#include <SProt/sprot_l.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

char invokes[4];
sprot_buff_entry* passed_ptr;
#define F01_OFF 0
#define F01_SIZE 2
#define F01_CMD 0x170
#define F02_OFF 1
#define F02_SIZE 0xFF
#define F02_CMD 0x11
#define F03_OFF 2
#define F03_SIZE 0
#define F03_CMD 0x70
#define FNULL_SIZE 1
#define FNULL_CMD 0x71
#define FDEF_OFF 3

#define START 0x5A


#define D_POLY_CRC8 0xCF
#define D_INITIAL_CRC8 0x55
uint8_t crc8(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly, bool lsbFirst, bool reversedCrc)
{
    poly &= 0xFF;
    sum &= 0xFF;
    assert(len>0);
    assert(poly!=0);

    if(reversedCrc)
    {
        uint8_t ncrc = 0;
        for(int i=0;i<8;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x80>>i;
        }
        sum = ncrc;
    }

    for(int i=0;i<len+1;++i)// dla każdego bajtu danych plus jeden bajt do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowy bajt do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];
        for(int i=0;i<8;++i)
        {
            bool needxor = sum&0x80;
            sum <<= 1;

            if(!lsbFirst)
            {
                if(lval&0x80)
                    sum |= 0x01;
                lval <<= 1;
            }
            else
            {
                if(lval&0x01)
                    sum |= 0x01;
                lval >>= 1;
            }

            if(needxor)
                sum ^= poly;
        }
    }

    if(reversedCrc)
    {
        uint8_t ncrc = 0;
        for(int i=0;i<8;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x80>>i;
        }
        sum = ncrc;
    }

    return sum&0xFF;
}

uint8_t crc(void* buff, uint8_t bytes)
{
	return crc8(buff, bytes, D_INITIAL_CRC8, D_POLY_CRC8, false, false);
}

void f01(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	invokes[F01_OFF]++;
	passed_ptr = buff;
}

void f02(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	invokes[F02_OFF]++;
	passed_ptr = buff;
}

void f03(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	invokes[F03_OFF]++;
	passed_ptr = buff;
}

void fdef(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	invokes[FDEF_OFF]++;
	passed_ptr = buff;
}

sprot_efunc tbl[] = {{.size=F01_SIZE, .cmd=F01_CMD, .fun_ptr=f01}, \
					 {.size=F02_SIZE, .cmd=F02_CMD, .fun_ptr=f02}, \
					 {.size=F03_SIZE, .cmd=F03_CMD, .fun_ptr=f03}, \
					 {.size=FNULL_SIZE, .cmd=FNULL_CMD, .fun_ptr=0}};
#define ENTR 4

void prepare_cmd(sprot_buff_entry* buff, uint16_t cmd, uint8_t size, bool invalid_offset, bool invalid_start, bool invalid_crc)
{
	buff->start = START;
	if(invalid_start)
		buff->start ^= 0xFF;
		
	buff->write_offseet = size+4;
	if(invalid_offset)
		buff->write_offseet -= 1;
		
	buff->cmdHSize = (cmd>>1)&0x80 | size&0x7F;
	buff->cmdL = cmd&0xFF;
	
	if(size>64)
		size = 64;
	
	for(int i=0;i<size;i++)
		buff->data_and_crc[i] = (i^cmd^size)&0xFF;
	
	buff->data_and_crc[size] = crc(&(buff->start), size+3);
	if(invalid_crc)
		buff->data_and_crc[size] ^= 0xFF;
}

void test_spexe_valid_commands()
{
	sprot_fifo fifo;
	
	// Valid command with long cmd
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={1, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid command with long cmd - proper command should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Valid command with long cmd - buff should be changed to SPROT_EMPTY");
	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
	"Valid command with long cmd - proper pointer should be passed");

	// Valid command with variable size, pt1
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 1, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 1, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid command with variable size, pt1 - proper command should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Valid command with variable size, pt1 - buff should be changed to SPROT_EMPTY");
	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
	"Valid command with variable size, pt1 - proper pointer should be passed");

	// Valid command with variable size, pt2
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 1, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 10, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid command with variable size, pt2 - proper command should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Valid command with variable size, pt2 - buff should be changed to SPROT_EMPTY");
	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
	"Valid command with variable size, pt2 - proper pointer should be passed");
	
	// Valid command with no data
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 1, 0};
	prepare_cmd(&(fifo.buffs[0]), F03_CMD, F03_SIZE, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid command with no data - proper command should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Valid command with no data - buff should be changed to SPROT_EMPTY");
	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
	"Valid command with no data - proper pointer should be passed");

	// Valid but unknown command
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected5[]={0, 0, 0, 1};
	prepare_cmd(&(fifo.buffs[0]), 0xAA, 5, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid but unknown command - default command should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Valid but unknown command - buff should be changed to SPROT_EMPTY");
	TEST_CHECK_P(passed_ptr==(sprot_buff_entry*)&(fifo.buffs[0]), \
	"Valid but unknown command - proper pointer should be passed");
}

void test_spexe_invalid_commands()
{
	sprot_fifo fifo;
	
	// Invalid crc
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, false, true);
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Invalid crc - no commands should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Invalid crc - buff should be changed to SPROT_EMPTY");
	
	// Invalid start byte
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, false, true, false);
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Invalid start byte - no commands should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Invalid start byte - buff should be changed to SPROT_EMPTY");
	
	// Invalid offset
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected3[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE, true, false, false);
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected3, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Invalid offset - no commands should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Invalid offset - buff should be changed to SPROT_EMPTY");
	
	// Invalid command size
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected4[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F01_CMD, F01_SIZE+1, false, false, false);
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected4, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Invalid command size - no commands should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Invalid command size - buff should be changed to SPROT_EMPTY");
	
	// Known command with variable size, cmd_size bigger than allowed
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected5[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), F02_CMD, 65, false, false, false);
	process_fifo(&fifo, 0, tbl, fdef, ENTR);
	TEST_CHECK_P(memcmp((void*)expected5, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Known command with variable size, cmd_size bigger than allowed - no "
	"commands should be executed");
	TEST_CHECK_P(fifo.buffs[0].status==SPROT_EMPTY, \
	"Known command with variable size, cmd_size bigger than allowed - buff "
	"should be changed to SPROT_EMPTY");
}

void test_spexe_null_ptrs()
{
	sprot_fifo fifo;
	
	// Valid command - empty pointer to function
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected1[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), FNULL_CMD, FNULL_SIZE, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, 0, ENTR);
	TEST_CHECK_P(memcmp((void*)expected1, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid command - empty pointer to function - no commands should be executed");
	
	// Valid but unknown command - empty pointer to function
	memset(&fifo, 0x00, sizeof(fifo));
	fifo.buffs[0].status = SPROT_FULL;
	memset(&invokes, 0x00, sizeof(invokes)/sizeof(invokes[0]));
	char expected2[]={0, 0, 0, 0};
	prepare_cmd(&(fifo.buffs[0]), 0xAA, 5, false, false, false);
	passed_ptr = 0;
	process_fifo(&fifo, 0, tbl, 0, ENTR);
	TEST_CHECK_P(memcmp((void*)expected2, (void*)invokes, sizeof(invokes)/sizeof(invokes[0]))==0, \
	"Valid but unknown command - empty pointer to function - no commands should be executed");
}

void test_spexe_crc()
{
	uint8_t tab1[] = {0x11, 0x00, 0x57, 0x91, 0x00};
	uint8_t s = sizeof(tab1)/sizeof(tab1[0]);
	TEST_CHECK_P(crc(tab1, s)==calc_crc(tab1, s), \
	"calc crc, data set 1");
	
	
	uint8_t tab2[] = {0x11, 0x00, 0x57, 0x91, 0x00};
	s = sizeof(tab2)/sizeof(tab2[0]);
	TEST_CHECK_P(crc(tab2, s)==calc_crc(tab2, s), \
	"calc crc, data set 1");
	
	
	uint8_t tab3[] = {0x11, 0x00, 0x57, 0x91, 0x00};
	s = sizeof(tab3)/sizeof(tab3[0]);
	TEST_CHECK_P(crc(tab3, s)==calc_crc(tab3, s), \
	"calc crc, data set 1");
	
	uint8_t tab4[] = {0x11, 0x00, 0x57, 0x91, 0x00};
	s = sizeof(tab4)/sizeof(tab4[0]);
	TEST_CHECK_P(crc(tab4, s)==calc_crc(tab4, s), \
	"calc crc, data set 1");
}

