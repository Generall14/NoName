#ifndef _CRC_H_
#define _CRC_H_

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define D_POLY_CRC8 0xCF
#define D_INITIAL_CRC8 0x55
uint8_t crc8(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly, bool lsbFirst, bool reversedCrc);
uint8_t crc(void* buff, uint8_t bytes);

#endif
