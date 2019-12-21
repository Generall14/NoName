#ifndef _TUTILS_H_
#define _TUTILS_H_

#ifdef UTEST
#define _GID
#define _GIE
#define STUB(fun) STUB_##fun
#else
#define _GID __disable_irq()
#define _GIE __enable_irq()
#define STUB(fun) fun
#endif

#endif
