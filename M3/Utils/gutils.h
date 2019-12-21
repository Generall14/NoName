#ifndef _GUTILS_H_
#define _GUTILS_H_

#define SWAP_BIT(REG, BIT)     ((REG) ^= (BIT))

#define LOBYTE(x)  ((uint8_t)(x & 0x00FF))
#define HIBYTE(x)  ((uint8_t)((x & 0xFF00) >>8))
#define MERGEWORD(HI,LO) ( (uint16_t)(((HI << 8)&0xFF00) | (LO &0xFF)) )
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#define CAT2(A, B) A##B
#define CAT3(A, B, C) A##B##C
#define CAT4(A, B, C, D) A##B##C##D

#endif
