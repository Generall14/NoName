#include "crc10.hpp"
#include <cassert>

/**
 * @author mgr inż. W. Kogut
 */

uint16_t crc10(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly, bool lsbFirst, bool reversedCrc)
{
    poly &= 0x3FF;
    assert(len>0);
    assert(poly!=0);

    if(reversedCrc)
    {
        uint16_t ncrc = 0;
        for(int i=0;i<10;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x200>>i;
        }
        sum = ncrc;
    }

    for(int i=0;i<len;++i)
    {
        uint8_t lval = ptr[i];
        for(int i=0;i<8;++i)
        {
            bool needxor = sum&0x200;
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

    for(int i=0;i<10;++i) // 10 dodatkowych bitów do wypchnięcia CRC
    {
        bool needxor = sum&0x200;
        sum <<= 1;
        if(needxor)
            sum ^= poly;
    }

    if(reversedCrc)
    {
        uint16_t ncrc = 0;
        for(int i=0;i<10;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x200>>i;
        }
        sum = ncrc;
    }

    return sum&0x3FF;
}

uint16_t crc10t(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly)
{
    poly &= 0x3FF;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc10table(poly);

    for(int i=0;i<len+1;++i) // dla każdego bajtu danych plus jeden bajt do przepchnięcia
    {
        uint8_t lval = 0x00;
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = (sum>>2)&0xFF;
        sum = (sum<<8)&0x0300;
        sum |= lval;
        sum ^= sp.get()[arrPtr];
    }

    uint8_t arrPtr = (sum>>8)&0x03; // dwa dodatkowe bity do wypchnięcia CRC
    sum = (sum<<2)&0x03FC;
    sum ^= sp.get()[arrPtr];

    return sum&0x3FF;
}

std::shared_ptr<uint16_t> makeCrc10table(uint16_t poly)
{
    poly &= 0x3FF;
    assert(poly!=0);

    std::shared_ptr<uint16_t> sp(new uint16_t[256], std::default_delete<uint16_t[]>());
    for(int i=0;i<256;++i)
    {
        int r = i<<2;
        for(int j=0;j<8;++j)
        {
            if((r&0x200)!=0)
                r = (r<<1)^poly;
            else
                r<<=1;
        }
        sp.get()[i] = r&0x3ff;
    }
    return sp;
}

uint16_t crc10tLsbFirst(const char* ptr, uint16_t len, uint16_t sum, uint16_t poly)
{
    poly &= 0x3FF;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc10tableLsbFirst(poly);

    for(int i=0;i<len+1;++i) // dla każdego bajtu danych plus jeden bajt do przepchnięcia
    {
        uint8_t lval = 0x00;
        if(i<len)
            lval = ptr[i];

        uint8_t arrPtr = sum&0xFF;
        sum = (sum>>8)&0x03;
        sum |= (lval<<2)&0x3FC;
        sum ^= sp.get()[arrPtr];
    }

    uint8_t arrPtr = (sum&0x03)<<6; // dwa dodatkowe bity do wypchnięcia CRC
    sum = (sum>>2)&0x00FF;
    sum ^= sp.get()[arrPtr];

    return sum&0x3FF;
}

std::shared_ptr<uint16_t> makeCrc10tableLsbFirst(uint16_t poly)
{
    poly &= 0x3FF;
    assert(poly!=0);

    uint16_t npoly = 0;
    for(int b=0;b<10;++b)
    {
        if(poly&(1<<b))
            npoly |= 0x200>>b;
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
        sp.get()[i] = r&0x3ff;
    }
    return sp;
}
