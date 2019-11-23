#include <iostream>
#include <iomanip>
#include <cassert>

#include <fstream>

#include "crc16.hpp"
#include "crc10.hpp"
#include "crc8.hpp"
#include "crc5.hpp"
#include "_crcPazur-Kopia.hpp"

#include <QString>

/**
 * @author mgr inż. W. Kogut
 */

using namespace std;

// Trochę funkcji i szablonów aby nie powtarzać w main() kilkunastu lini kodu dla każdego przypadku testowego:
void displayCalcs(uint16_t poly, uint16_t crc0, bool lsbFirst, bool reversedCrc, uint16_t calculatedCrc, bool table=false)
{
    cout << "Poly: " << poly << ", crc0: " << crc0 << ", lsbFirst: " << lsbFirst << ", reversedCrc: " << reversedCrc << endl;
    if(table)
        cout << "Calculated crc (table method): ";
    else
        cout << "Calculated crc: ";
    cout << calculatedCrc << endl << endl;
}

template<typename T>
struct ptrs
{
    typedef T (*standardCRC)(const char*, uint16_t, T, T, bool, bool);
    typedef T (*tableCRC)(const char*, uint16_t, T, T);
    typedef T (*invertedtableCRC)(const char*, uint16_t, T, T);
};

template<typename T>
void doSomeStuff(const char* ptr, uint16_t size, T poly, T crc0,
                 typename ptrs<T>::standardCRC s, typename ptrs<T>::tableCRC t,
                 typename ptrs<T>::invertedtableCRC it)
{
    auto returned = (*s)(ptr, size, crc0, poly, false, false);
    displayCalcs(poly, crc0, false, false, returned);
    returned = (*t)(ptr, size, crc0, poly);
    displayCalcs(poly, crc0, false, false, returned, true);

    returned = (*s)(ptr, size, crc0, poly, true, true);
    displayCalcs(poly, crc0, true, true, returned);
    returned = (*it)(ptr, size, crc0, poly);
    displayCalcs(poly, crc0, true, true, returned, true);
}

int main()
{
//    char dat[] = "All";
//    dat[0] = 0x50;
//    dat[1] = 0x00;
//    dat[2] = 0x52;
//    const uint16_t size = (sizeof(dat)/sizeof(*dat))-1;
//    assert(size!=0xFFFF);
//    cout << hex << showbase;
//    cout << "Input data: \"" << dat << "\"\n\n";

//    //========================== CRC16 ===========================================================
//    uint16_t poly = 0x1021;
//    uint16_t crc0 = 0x5555;
//    cout << "========================== CRC16 ==========================\n\n";
//    doSomeStuff(dat, size, poly, crc0, &crc16, &crc16t, &crc16tLsbFirst);

//    //========================== CRC10 ===========================================================
//    poly = 0x64f;
//    crc0 = 0x5555;
//    cout << "========================== CRC10 ==========================\n\n";
//    doSomeStuff(dat, size, poly, crc0, &crc10, &crc10t, &crc10tLsbFirst);

//    //========================== CRC8 = ===========================================================
//    uint8_t poly8 = 0xCF;
//    uint8_t crc08 = 0x55;
//    cout << "========================== CRC8 ===========================\n\n";
//    doSomeStuff(dat, size, poly8, crc08, &crc8, &crc8t, &crc8tLsbFirst);

//    //========================== CRC5 = ===========================================================
//    poly8 = 0x1D;
//    crc08 = 0x05;
//    cout << "========================== CRC5 ===========================\n\n";
//    doSomeStuff(dat, size, poly8, crc08, &crc5, &crc5t, &crc5tLsbFirst);

//    std::shared_ptr<uint16_t> tab10 = makeCrc10tableLsbFirst();
//    std::shared_ptr<uint8_t> tab8 = makeCrc8tableLsbFirst();
//    std::shared_ptr<uint8_t> tab5 = makeCrc5tableLsbFirst();

//    ofstream f10("tab10.txt", ofstream::out | ofstream::trunc);
//    for(int i=0;i<256;++i)
//    {
//        f10 << "0x" << QString("%1").arg(tab10.get()[i]&0xffff, 4, 16, QChar('0')).toUpper().toStdString();
//        if(i!=255)
//            f10 << ", ";
//    }
//    f10.close();

//    ofstream f8("tab8.txt", ofstream::out | ofstream::trunc);
//    for(int i=0;i<256;++i)
//    {
//        f8 << "0x" << QString("%1").arg(tab8.get()[i]&0xff, 2, 16, QChar('0')).toUpper().toStdString();
//        if(i!=255)
//            f8 << ", ";
//    }
//    f8.close();

//    ofstream f5("tab5.txt", ofstream::out | ofstream::trunc);
//    for(int i=0;i<16;++i)
//    {
//        f5 << "0x" << QString("%1").arg(tab5.get()[i]&0xff, 2, 16, QChar('0')).toUpper().toStdString();
//        if(i!=15)
//            f5 << ", ";
//    }
//    f5.close();




    std::cout << "\n\n";
    char dat2[] = "1234";
    dat2[0] = 0x37;
    dat2[1] = 0x25;
    dat2[2] = 0x00;
    dat2[3] = 0x00;
    const uint16_t size2 = (sizeof(dat2)/sizeof(*dat2))-2;
    uint16_t ret =  crc5tLsbFirst(dat2, size2);
    std::cout << "crc5: " << hex << showbase << (ret<<2) << std::endl;

    char* table_ptr5 = new char[100];
    _make_crc_4bit_table(table_ptr5, 0x1D);
    ret = _crc5(dat2, size2, 0x14, table_ptr5);
    ret = _crc5_push_bit(dat2[3]&0x01, ret, table_ptr5);
    ret = _crc5_push_bit((dat2[3]>>1)&0x01, ret, table_ptr5);
    for(int i=0;i<5;i++)
        ret = _crc5_push_bit(0, ret, table_ptr5);
    std::cout << "crc5: " << hex << showbase << (ret<<2) << std::endl;

    std::cout << "\n\n";
    char dat8[100];
    dat8[0] = 0x11;
    dat8[1] = 0x00;
    dat8[2] = 0x01;
    dat8[3] = 0x12;
    dat8[4] = 0x01;
    dat8[5] = 0x01;
    dat8[6] = 0x02;
    dat8[7] = 0x13;
    dat8[8] = 0x02;
    dat8[9] = 0x01;
    dat8[10] = 0x02;
    dat8[11] = 0x03;
    const uint16_t size8 = 12;
    uint16_t ret8 =  crc8tLsbFirst(dat8, size8);
    std::cout << "crc8: " << hex << showbase << ret8 << std::endl;


    std::cout << "\n\n";
    char dat10[100];
    dat10[0] = 0x12;
    dat10[1] = 0x01;
    dat10[2] = 0x01;
    dat10[3] = 0x02;

    const uint16_t size10 = 4;
    uint16_t ret10 =  crc10tLsbFirst(dat10, size10);
    std::cout << "crc10: " << hex << showbase << ret10 << std::endl;

    return 0;
}
