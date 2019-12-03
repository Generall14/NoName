#include "sprot_i.h"
#include "usbd_cdc_if.h"

sprot_fifo pc_fifo;
sprot_fifo rpc_fifo;

sprot_efunc ififo_tbl[] = {\
		{.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}, \
		{.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}
};

sprot_section spt_sec_tbl[] = {\
		{.number=0, .data_ptr=sec_test, .bytes=4, .fun_read_cpy=mmcpy, .fun_write_cpy=mmcpy},\
		{.number=1, .data_ptr=(uint8_t*)(&sec_sysTime), .bytes=4, .fun_read_cpy=mmcpy, .fun_write_cpy=0}
};

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
	memcpy(dest, src, bytes);
}

uint8_t sec_test[] = {'N', 'I', 'C', '!'};
uint32_t sec_sysTime = 0;
