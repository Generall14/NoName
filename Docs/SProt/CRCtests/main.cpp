#include <iostream>
#include <iomanip>
#include <cassert>

/**
 * File generates table of uint8_t values using in calculating CRC on uC. Prints it to 'tab8.txt'
**/

#include <fstream>

#include "crc8.hpp"

using namespace std;

int main()
{
	std::shared_ptr<uint8_t> tab8 = makeCrc8table();

	ofstream f8("tab8.txt", ofstream::out | ofstream::trunc);
	f8 << "{";
	for(int i=0;i<256;++i)
	{
		f8 << hex << "0x" << (uint)tab8.get()[i];
		if(i!=255)
			f8 << ", ";
	}
	f8 << "};";
	f8.close();

	return 0;
}
