#include "crc5.hpp"
#include <cassert>

/**
 * @author mgr inż. W. Kogut
 */

uint8_t crc5(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly, bool lsbFirst, bool reversedCrc)
{
    poly &= 0x1F;
    assert(len>0);
    assert(poly!=0);

    if(reversedCrc)
    {
        uint8_t ncrc = 0;
        for(int i=0;i<5;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x10>>i;
        }
        sum = ncrc;
    }

    for(int i=0;i<len;++i)
    {
        uint8_t lval = ptr[i];
        for(int i=0;i<8;++i)
        {
            bool needxor = sum&0x10;
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

    for(int i=0;i<5;++i) // dodatkowe 5 bitów do wypchnięcia CRC
    {
        bool needxor = sum&0x10;
        sum <<= 1;
        if(needxor)
            sum ^= poly;
    }

    if(reversedCrc)
    {
        uint8_t ncrc = 0;
        for(int i=0;i<5;++i)
        {
            if(sum&(1<<i))
                ncrc |= 0x10>>i;
        }
        sum = ncrc;
    }

    return sum&0x1F;
}

uint8_t crc5t(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly)
{
    poly &= 0x1F;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc5table(poly);

    for(int i=0;i<len;++i)
    {
        uint8_t lval = ptr[i];

        uint8_t arrPtr = (sum>>1)&0xF;
        sum = (sum<<4)&0x10;
        sum |= (lval>>4)&0x0F;
        sum ^= sp.get()[arrPtr];

        arrPtr = (sum>>1)&0xF;
        sum = (sum<<4)&0x10;
        sum |= lval&0x0F;
        assert(arrPtr<=0x0F);
        sum ^= sp.get()[arrPtr];
    }

    uint8_t arrPtr = (sum>>1)&0xF; // dodatkowa połówka do wypchnięcia CRC
    sum = (sum<<4)&0x10;
    assert(arrPtr<=0x0F);
    sum ^= sp.get()[arrPtr];

    arrPtr = (sum>>4)&0x01; // ostatni bit do wypchnięcia CRC
    sum = (sum<<1)&0x1E;
    assert(arrPtr<=0x0F);
    sum ^= sp.get()[arrPtr];

    return sum&0x1F;
}

std::shared_ptr<uint8_t> makeCrc5table(uint8_t poly)
{
    poly &= 0x1F;
    assert(poly!=0);

    std::shared_ptr<uint8_t> sp(new uint8_t[16], std::default_delete<uint8_t[]>());
    for(int i=0;i<16;++i)
    {
        int r = i<<1;
        for(int j=0;j<4;++j)
        {
            if((r&0x10)!=0)
                r = (r<<1)^poly;
            else
                r<<=1;
        }
        sp.get()[i] = r&0x1f;
    }
    return sp;
}

uint8_t crc5tLsbFirst(const char* ptr, uint16_t len, uint8_t sum, uint8_t poly)
{
    poly &= 0x1F;
    assert(poly!=0);
    assert(len>0);
    auto sp = makeCrc5tableLsbFirst(poly);

    for(int i=0;i<len;++i)
    {
        uint8_t lval = ptr[i];

        uint8_t arrPtr = sum&0x0F;
        sum = (sum>>4)&0x01;
        sum |= (lval<<1)&0x1E;
        sum ^= sp.get()[arrPtr];

        arrPtr = sum&0x0F;
        sum = (sum>>4)&0x01;
        sum |= (lval>>3)&0x1E;
        assert(arrPtr<=0x0F);
        sum ^= sp.get()[arrPtr];
    }

    uint8_t arrPtr = sum&0x0F; // dodatkowa połówka do wypchnięcia CRC
    sum = (sum>>4)&0x01;
    assert(arrPtr<=0x0F);
    sum ^= sp.get()[arrPtr];

    arrPtr = sum&0x01; // ostatni bit do wypchnięcia CRC
    sum = (sum>>1)&0x0F;
    assert(arrPtr<=0x0F);
    sum ^= sp.get()[arrPtr<<3];

    return sum&0x1F;
}

std::shared_ptr<uint8_t> makeCrc5tableLsbFirst(uint8_t poly)
{
    poly &= 0x1F;
    assert(poly!=0);

    uint8_t npoly = 0;
    for(int b=0;b<5;++b)
    {
        if(poly&(1<<b))
            npoly |= 0x10>>b;
    }
    poly = npoly;

    std::shared_ptr<uint8_t> sp(new uint8_t[16], std::default_delete<uint8_t[]>());
    for(int i=0;i<16;++i)
    {
        int r = i;
        for(int j=0;j<4;++j)
        {
            if((r&0x01)!=0)
                r = (r>>1)^poly;
            else
                r>>=1;
        }
        sp.get()[i] = r&0x1f;
    }
    return sp;
}
