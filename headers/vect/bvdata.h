#pragma once

#include <array>

#include "bfa.h"

#define BV8(constant) static_cast<bf::bv8>(UINT8_C(constant))
#define BV16(constant) static_cast<bf::bv16>(UINT16_C(constant))
#define BV32(constant) static_cast<bf::bv32>(UINT32_C(constant))
#define BV64(constant) static_cast<bf::bv64>(UINT64_C(constant))

namespace bf
{
  typedef uint8_t bv8;
  typedef uint16_t bv16;
  typedef uint32_t bv32;
  typedef uint64_t bv64;

  /*
  constexpr std::array<bv64, 2>  _minAndMax = { BV64(0), BV64(0xFFFFFFFFFFFFFFFF) };

  constexpr std::array<bv64, 65> _max = {
    BV64(0),
    BV64(0x1), BV64(0x3), BV64(0x7), BV64(0xF),
    BV64(0x1F), BV64(0x3F), BV64(0x7F), BV64(0xFF),
    BV64(0x1FF), BV64(0x3FF), BV64(0x7FF), BV64(0xFFF),
    BV64(0x1FFF), BV64(0x3FFF), BV64(0x7FFF), BV64(0xFFFF),
    BV64(0x1FFFF), BV64(0x3FFFF), BV64(0x7FFFF), BV64(0xFFFFF),
    BV64(0x1FFFFF), BV64(0x3FFFFF), BV64(0x7FFFFF), BV64(0xFFFFFF),
    BV64(0x1FFFFFF), BV64(0x3FFFFFF), BV64(0x7FFFFFF), BV64(0xFFFFFFF),
    BV64(0x1FFFFFFF), BV64(0x3FFFFFFF), BV64(0x7FFFFFFF), BV64(0xFFFFFFFF),
    BV64(0x1FFFFFFFF), BV64(0x3FFFFFFFF), BV64(0x7FFFFFFFF), BV64(0xFFFFFFFFF),
    BV64(0x1FFFFFFFFF), BV64(0x3FFFFFFFFF), BV64(0x7FFFFFFFFF), BV64(0xFFFFFFFFFF),
    BV64(0x1FFFFFFFFFF), BV64(0x3FFFFFFFFFF), BV64(0x7FFFFFFFFFF), BV64(0xFFFFFFFFFFF),
    BV64(0x1FFFFFFFFFFF), BV64(0x3FFFFFFFFFFF), BV64(0x7FFFFFFFFFFF), BV64(0xFFFFFFFFFFFF),
    BV64(0x1FFFFFFFFFFFF), BV64(0x3FFFFFFFFFFFF), BV64(0x7FFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFF),
    BV64(0x1FFFFFFFFFFFFF), BV64(0x3FFFFFFFFFFFFF), BV64(0x7FFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFF),
    BV64(0x1FFFFFFFFFFFFFF), BV64(0x3FFFFFFFFFFFFFF), BV64(0x7FFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFF),
    BV64(0x1FFFFFFFFFFFFFFF), BV64(0x3FFFFFFFFFFFFFFF), BV64(0x7FFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF)
  };

  constexpr std::array<bv64, 33> _maxPow = {
    BV64(0x1),
    BV64(0x3), BV64(0xF), BV64(0xFF), BV64(0xFFFF),
    BV64(0xFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF),
    BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF), BV64(0xFFFFFFFFFFFFFFFF)
  };
  */
  
  constexpr bv64 _deBruijnSequence = BV64(0x218A392CD3D5DBF);

  constexpr std::array<bv8, 64> _deBruijnWordsInversed = {
    0, 1, 2, 7, 3, 13, 8, 19, 4, 25, 14, 28, 9, 34, 20, 40, 
    5, 17, 26, 38, 15, 46, 29, 48, 10, 31, 35, 54, 21, 50, 41, 57, 
    63, 6, 12, 18, 24, 27, 33, 39, 16, 37, 45, 47, 30, 53, 49, 56, 
    62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58
  };

  /*
  vectorOfWeightAtMost1 should be a power of 2 or 0 (returns 0 for 0);
*/
  inline int trailingZeros(bv64 vectorOfWeightAtMost1)
  {
    return _deBruijnWordsInversed[(vectorOfWeightAtMost1 * _deBruijnSequence) >> 58];
  }

  constexpr std::array<bv8, 256> _weightTable = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
  };
}
