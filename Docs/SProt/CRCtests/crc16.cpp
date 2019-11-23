#include "crc16.hpp"
#include <cassert>

/**
 * @author mgr inż. W. Kogut
 */

uint16_t crc16(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly, bool lsbFirst, bool reversedCrc)
{
    assert(len>0);
    assert(poly!=0);

    if(reversedCrc)
    {
        uint16_t ncrc = 0;
        for(int i=0;i<16;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x8000>>i;
        }
        sum = ncrc;
    }

    for(int i=0;i<len+2;++i) // dla każdego bajtu danych plus dwa bajty do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowe bajty do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];
        for(int i=0;i<8;++i)
        {
            bool needxor = sum&0x8000;
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
        uint16_t ncrc = 0;
        for(int i=0;i<16;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x8000>>i;
        }
        sum = ncrc;
    }

    return sum&0xFFFF;
}

uint16_t crc16t(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly)
{
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc16table(poly);

    for(int i=0;i<len+2;++i) // dla każdego bajtu danych plus dwa bajty do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowe bajty do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = (sum>>8)&0xFF;
        sum = (sum<<8)&0xFF00;
        sum |= lval;
        sum ^= sp.get()[arrPtr];
    }

    return sum&0xFFFF;
}

std::shared_ptr<uint16_t> makeCrc16table(uint16_t poly)
{
    assert(poly!=0);

    std::shared_ptr<uint16_t> sp(new uint16_t[256], std::default_delete<uint16_t[]>());
    for(int i=0;i<256;++i)
    {
        int r = i<<8;
        for(int j=0;j<8;++j)
        {
            if((r&0x8000)!=0)
                r = (r<<1)^poly;
            else
                r<<=1;
        }
        sp.get()[i] = r&0xffff;
    }
    return sp;
}

uint16_t crc16tLsbFirst(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly)
{
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc16tableLsbFirst(poly);

    for(int i=0;i<len+2;++i) // dla każdego bajtu danych plus dwa bajty do przepchnięcia
    {
        uint8_t lval = 0x00; // zerowe bajty do wypchnięcia CRC
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = sum&0xFF;
        sum = (sum>>8)&0xFF;
        sum |= (lval<<8)&0xFF00;
        sum ^= sp.get()[arrPtr];
    }

    return sum&0xFFFF;
}

std::shared_ptr<uint16_t> makeCrc16tableLsbFirst(uint16_t poly)
{
    assert(poly!=0);

    uint16_t npoly = 0;
    for(int b=0;b<16;++b)
    {
        if(poly&(1<<b))
            npoly |= 0x8000>>b;
    }
    poly = npoly;

    std::shared_ptr<uint16_t> sp(new uint16_t[256], std::default_delete<uint16_t[]>());
    for(int i=0;i<256;++i)
    {
        int r = i;
        for(int j=0;j<8;++j)
        {
            if((r&0x0001)!=0)
                r = (r>>1)^poly;
            else
                r>>=1;
        }
        sp.get()[i] = r&0xffff;
    }
    return sp;
}
