#ifndef _SPROT_I_H_
#define _SPROT_I_H_

#include "stm32f1xx_hal.h"
#include "sprot_l.h"

// Data from PC

void ififo_def(sprot_buff_entry* buff);

#define HELLO_CMD 0x100
#define HELLO_SIZE 0
void ififo_hello(sprot_buff_entry* buff);

extern sprot_fifo pc_fifo;
extern sprot_efunc ififo_tbl[];
extern const uint16_t ififo_tbl_size;

// Data to PC

void irfifo_def(sprot_buff_entry* buff);

extern sprot_fifo rpc_fifo;
extern sprot_efunc irfifo_tbl[];
#define irfifo_tbl_size 0

#endif
