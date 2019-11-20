#include "header.h"

#include <SProt/sprot_l.h>
#include <stdbool.h>
#include <string.h>

char invokes[4];
#define F01_OFF 0
#define F01_SIZE 2
#define F01_CMD 0x170
#define F02_OFF 1
#define F02_SIZE 5
#define F02_CMD 0x11
#define F03_OFF 2
#define F03_SIZE 0
#define F03_CMD 0x70
#define FDEF_OFF 3

void f01(sprot_buff_entry* buff)
{
	invokes[F01_OFF]++;
}

void f02(sprot_buff_entry* buff)
{
	invokes[F02_OFF]++;
}

void f03(sprot_buff_entry* buff)
{
	invokes[F03_OFF]++;
}

void fdef(sprot_buff_entry* buff)
{
	invokes[FDEF_OFF]++;
}

sprot_efunc tbl[] = {{.size=F01_SIZE, .cmd=F01_CMD, .fun_ptr=f01}, \
					 {.size=F02_SIZE, .cmd=F02_CMD, .fun_ptr=f02}, \
					 {.size=F03_SIZE, .cmd=F03_CMD, .fun_ptr=f03}};
