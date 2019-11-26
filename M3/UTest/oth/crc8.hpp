#ifndef CRC8_HPP
#define CRC8_HPP

#include <stdint.h>
#include <memory>

/**
 * @author mgr inż. W. Kogut
 */

#define D_POLY_CRC8 0xCF
#define D_INITIAL_CRC8 0x55

/**
 * Oblicza crc8 sposobem łopatologicznym, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @param lsbFirst - false: bity bajtów danych będą pompowane zaczynając od najbardziej znaczących.
 * @param reversedCrc - true: po zakończeniu obliczeń zostanie zamieniona kolejność bitów sumy crc.
 * @return wynik obliczeń.
 */
uint8_t crc8(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC8, uint8_t poly=D_POLY_CRC8, bool lsbFirst=false, bool reversedCrc=false);



/**
 * Oblicza crc8 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc. Bity pompowane od najbardziej znaczących, suma nie odwracana.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint8_t crc8t(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC8, uint8_t poly=D_POLY_CRC8);
/**
 * Tworzy tablicę do obliczania crc8 (dla funkcji crc8t) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint8_t> makeCrc8table(uint8_t poly=D_POLY_CRC8);



/**
 * Oblicza crc8 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc. Bity pompowane od najmniej znaczących, suma odwracana bitowo.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint8_t crc8tLsbFirst(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC8, uint8_t poly=D_POLY_CRC8);
/**
 * Tworzy tablicę do obliczania crc8 (dla funkcji crc8tLsbFirst) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint8_t> makeCrc8tableLsbFirst(uint8_t poly=D_POLY_CRC8);

#endif
