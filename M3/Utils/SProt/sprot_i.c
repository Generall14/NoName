#include "sprot_i.h"
#include "usbd_cdc_if.h"

sprot_fifo pc_fifo;
sprot_fifo rpc_fifo;

sprot_efunc ififo_tbl[] = {\
	{.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}, \
	{.size=GETSEC_SIZE, .cmd=GETSEC_CMD, .fun_ptr=sprot_read_sec}, \
	{.size=SETSEC_SIZE, .cmd=SETSEC_CMD, .fun_ptr=sprot_write_sec}
};

sprot_section spt_sec_tbl[] = {\
	{.number=0, .data_ptr=sec_test, .bytes=4, .fun_read_cpy=mmcpy, .fun_write_cpy=mmcpy},\
	{.number=1, .data_ptr=(uint8_t*)(&sec_sysTime), .bytes=4, .fun_read_cpy=mmcpy, .fun_write_cpy=0},\
	{.number=2, .data_ptr=sec_ver, .bytes=4, .fun_read_cpy=mmcpy, .fun_write_cpy=0},\
	{.number=3, .data_ptr=sec_commitId, .bytes=COMMIT_ID_BYTES, .fun_read_cpy=mmcpy, .fun_write_cpy=0},\
	{.number=4, .data_ptr=sec_commitDate, .bytes=COMMIT_DATE_BYTES, .fun_read_cpy=mmcpy, .fun_write_cpy=0}
};
uint8_t spt_sec_tbl_entries = sizeof(spt_sec_tbl)/sizeof(spt_sec_tbl[0]);

sprot_efunc irfifo_tbl[] = {};

const uint16_t ififo_tbl_size = sizeof(ififo_tbl)/sizeof(ififo_tbl[0]);

void ififo_def(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{

}

void ififo_hello(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	sp_push_command_to_fifo(re_fifo, CMD_REHELLO, "SProt-M3-dev", 12);
}

void irfifo_def(sprot_buff_entry* buff, sprot_fifo* re_fifo)
{
	while(CDC_Transmit_FS(&(buff->start), (buff->cmdHSize&0x7F)+4)==USBD_BUSY);
}

void mmcpy(uint8_t* dest, uint8_t* src, uint8_t bytes)
{
	for(int i =0; i<bytes; i++)
		dest[i] = src[i];
	//memcpy(dest, src, bytes);
}

uint8_t sec_test[] = {'N', 'I', 'C', '!'};
uint32_t sec_sysTime = 0;
uint8_t sec_ver[] = {MAJOR, MINOR, LOBYTE(BUILD), HIBYTE(BUILD)};
uint8_t sec_commitId[] = COMMIT_ID;
uint8_t sec_commitDate[] = COMMIT_DATE;
