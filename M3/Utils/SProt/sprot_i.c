#include "sprot_i.h"
#include "usbd_cdc_if.h"

sprot_fifo pc_fifo;
sprot_fifo rpc_fifo;

sprot_efunc ififo_tbl[] = {{.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}, \
					 	   {.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}};

sprot_efunc irfifo_tbl[] = {};

const uint16_t ififo_tbl_size = sizeof(ififo_tbl)/sizeof(ififo_tbl[0]);

void ififo_def(sprot_buff_entry* buff)
{

}

void ififo_hello(sprot_buff_entry* buff)
{
	sp_push_command_to_fifo(&rpc_fifo, CMD_REHELLO, "SProt-M3-dev", 12);
}

void irfifo_def(sprot_buff_entry* buff)
{
	while(CDC_Transmit_FS(&(buff->start), (buff->cmdHSize&0x7F)+4)==USBD_BUSY);
}
