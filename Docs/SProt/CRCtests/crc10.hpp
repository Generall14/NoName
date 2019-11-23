#ifndef CRC10_HPP
#define CRC10_HPP

#include <stdint.h>
#include <memory>

/**
 * @author mgr inż. W. Kogut
 */

#define D_POLY_CRC10 0x64F
#define D_INITIAL_CRC10 0x155

/**
 * Oblicza crc10 sposobem łopatologicznym, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych bajty zerowe
 * przepychające crc.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @param lsbFirst - false: bity bajtów danych będą pompowane zaczynając od najbardziej znaczących.
 * @param reversedCrc - true: po zakończeniu obliczeń zostanie zamieniona kolejność bitów sumy crc.
 * @return wynik obliczeń.
 */
uint16_t crc10(const char* ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC10, uint16_t poly=D_POLY_CRC10, bool lsbFirst=false, bool reversedCrc=false);



/**
 * Oblicza crc10 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc. Bity pompowane od najbardziej znaczących, suma nie odwracana.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint16_t crc10t(const char* ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC10, uint16_t poly=D_POLY_CRC10);
/**
 * Tworzy tablicę do obliczania crc10 (dla funkcji crc10t) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint16_t> makeCrc10table(uint16_t poly=D_POLY_CRC10);



/**
 * Oblicza crc16 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o dwa bajty zerowe
 * przepychające crc. Bity pompowane od najmniej znaczących, suma odwracana bitowo.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint16_t crc10tLsbFirst(const char* ptr, uint16_t len, uint16_t sum = D_INITIAL_CRC10, uint16_t poly=D_POLY_CRC10);
/**
 * Tworzy tablicę do obliczania crc16 (dla funkcji crc16tLsbFirst) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint16_t> makeCrc10tableLsbFirst(uint16_t poly=D_POLY_CRC10);

#endif
