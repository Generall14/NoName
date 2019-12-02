#include "crc.h"

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
