#include "sprot_i.h"

sprot_fifo pc_fifo;

sprot_efunc ififo_tbl[] = {{.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}, \
					 	   {.size=HELLO_SIZE, .cmd=HELLO_CMD, .fun_ptr=ififo_hello}};

const uint16_t ififo_tbl_size = sizeof(ififo_tbl)/sizeof(ififo_tbl[0]);

void ififo_def(sprot_buff_entry* buff)
{

}

void ififo_hello(sprot_buff_entry* buff)
{

}
