#include "crc8.hpp"
#include <cassert>

/**
 * @author mgr inż. W. Kogut
 */

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

uint8_t crc8t(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly)
{
    poly &= 0xFF;
    sum &= 0xFF;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc8table(poly);

    for(int i=0;i<len+1;++i) // dla każdego bajtu danych plus jeden bajt do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowy bajt do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = sum&0xFF;
        sum = lval;
        sum ^= sp.get()[arrPtr];
    }

    return sum&0xFF;
}

std::shared_ptr<uint8_t> makeCrc8table(uint8_t poly)
{
    poly &= 0xFF;
    assert(poly!=0);

    std::shared_ptr<uint8_t> sp(new uint8_t[256], std::default_delete<uint8_t[]>());
    for(int i=0;i<256;++i)
    {
        int r = i;
        for(int j=0;j<8;++j)
        {
            if((r&0x80)!=0)
                r = (r<<1)^poly;
            else
                r<<=1;
        }
        sp.get()[i] = r&0xff;
    }
    return sp;
}

uint8_t crc8tLsbFirst(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly)
{
    poly &= 0xFF;
    sum &= 0xFF;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc8tableLsbFirst(poly);

    for(int i=0;i<len+1;++i) // dla każdego bajtu danych plus jeden bajt do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowy bajt do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = sum&0xFF;
        sum = lval;
        sum ^= sp.get()[arrPtr];
    }

    return sum&0xFF;
}

std::shared_ptr<uint8_t> makeCrc8tableLsbFirst(uint8_t poly)
{
    poly &= 0xFF;
    assert(poly!=0);

    uint16_t npoly = 0;
    for(int b=0;b<8;++b)
    {
        if(poly&(1<<b))
            npoly |= 0x80>>b;
    }
    poly = npoly;

    std::shared_ptr<uint8_t> sp(new uint8_t[256], std::default_delete<uint8_t[]>());
    for(int i=0;i<256;++i)
    {
        int r = i;
        for(int j=0;j<8;++j)
        {
            if((r&0x01)!=0)
                r = (r>>1)^poly;
            else
                r>>=1;
        }
        sp.get()[i] = r&0xff;
    }
    return sp;
}
