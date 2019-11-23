#ifndef CRC5_HPP
#define CRC5_HPP

#include <stdint.h>
#include <memory>

/**
 * @author mgr inż. W. Kogut
 */

#define D_POLY_CRC5 0x1D
#define D_INITIAL_CRC5 0x14

/**
 * Oblicza crc5 sposobem łopatologicznym, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @param lsbFirst - false: bity bajtów danych będą pompowane zaczynając od najbardziej znaczących.
 * @param reversedCrc - true: po zakończeniu obliczeń zostanie zamieniona kolejność bitów sumy crc.
 * @return wynik obliczeń.
 */
uint8_t crc5(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC5, uint8_t poly=D_POLY_CRC5, bool lsbFirst=false, bool reversedCrc=false);



/**
 * Oblicza crc5 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc. Bity pompowane od najbardziej znaczących, suma nie odwracana.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint8_t crc5t(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC5, uint8_t poly=D_POLY_CRC5);
/**
 * Tworzy tablicę do obliczania crc8 (dla funkcji crc5t) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint8_t> makeCrc5table(uint8_t poly=D_POLY_CRC5);



/**
 * Oblicza crc5 metodą tablicową, nie nadaje się do rekurencyjnego doliczania crc, uzupełnia wejściowy ciąg danych o bajty zerowe
 * przepychające crc. Bity pompowane od najmniej znaczących, suma odwracana bitowo.
 * @param ptr - wskaźnik na dane.
 * @param len - długośc danych (w bajtach).
 * @param sum - początkowa wartość sumy.
 * @param poly - wielomian.
 * @return wynik obliczeń.
 */
uint8_t crc5tLsbFirst(const char* ptr, uint16_t len, uint8_t sum = D_INITIAL_CRC5, uint8_t poly=D_POLY_CRC5);
/**
 * Tworzy tablicę do obliczania crc8 (dla funkcji crc5tLsbFirst) metodą tablicową.
 * @param poly - wielomian.
 */
std::shared_ptr<uint8_t> makeCrc5tableLsbFirst(uint8_t poly=D_POLY_CRC5);

#endif
