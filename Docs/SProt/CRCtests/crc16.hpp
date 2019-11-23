#ifndef CRC16_HPP
#define CRC16_HPP

#include <stdint.h>
#include <memory>

/**
 * @author mgr inż. W. Kogut
 */

#define D_POLY_CRC16 0x1021
#define D_INITIAL_CRC16 0x5555

/**
 * Oblicza crc16 sposobem łopatologicznym, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o dwa bajty zerowe
 * przepychające crc.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @param lsbFirst - false: bity bajtów danych będą pompowane zaczynając od najbardziej znaczących.
 * @param reversedCrc - true: po zakończeniu obliczeń zostanie zamieniona kolejność bitów sumy crc.
 * @return wynik obliczeń.
 */
uint16_t crc16(const char *ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC16, uint16_t poly=D_POLY_CRC16, bool lsbFirst=false, bool reversedCrc=false);



/**
 * Oblicza crc16 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o dwa bajty zerowe
 * przepychające crc. Bity pompowane od najbardziej znaczących, suma nie odwracana.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint16_t crc16t(const char* ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC16, uint16_t poly=D_POLY_CRC16);
/**
 * Tworzy tablicę do obliczania crc16 (dla funkcji crc16t) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint16_t> makeCrc16table(uint16_t poly=D_POLY_CRC16);



/**
 * Oblicza crc16 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o dwa bajty zerowe
 * przepychające crc. Bity pompowane od najmniej znaczących, suma odwracana bitowo.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint16_t crc16tLsbFirst(const char* ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC16, uint16_t poly=D_POLY_CRC16);
/**
 * Tworzy tablicę do obliczania crc16 (dla funkcji crc16tLsbFirst) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint16_t> makeCrc16tableLsbFirst(uint16_t poly=D_POLY_CRC16);

#endif
