#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"
#include "vect/bv8.h"

namespace bf
{
  inline bv32 max32(bv8 n)
  {
    assert(n <= 32);
    return static_cast<bv32>(~(UINT64_C(0xFFFFFFFF) << n));
  }

  inline bv32 basis32(bv8 i)
  {
    assert(i < 32);
    return (BV32(1) << i);
  }

  inline bv8 lead(bv32 vect)
  {
    bv8 result = BV8(0);
    
    bv32 tmp = vect >> 16;
    if (tmp != 0) { vect = tmp; result += BV8(16); }

    tmp = vect >> 8;
    if (tmp != 0) { vect = tmp; result += BV8(8); }

    return result + lead(static_cast<bv8>(vect));
  }

  inline bv8 size(bv32 vect)
  {
    return lead(vect) + 1;
  }

  inline bv8 weight(bv32 vect)
  {
    return _weightTable[BV8(0xFF) & vect] +
      _weightTable[BV8(0xFF) & (vect >> 8)] +
      _weightTable[BV8(0xFF) & (vect >> 16)] +
      _weightTable[BV8(0xFF) & (vect >> 24)];
  }

  inline bool odd(bv32 vect)
  {
    vect ^= vect >> 16;
    vect ^= vect >> 8;
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return (vect & BV32(1)) != 0;
  }

  inline bool get(bv32 vect, bv8 idx)
  {
    assert(idx < 32);
    return (bool)((vect >> idx) & BV32(1));
  }


  inline void set0(bv32 &vect, bv8 idx)
  {
    assert(idx < 32);
    vect &= ~(BV32(1) << idx);
  }

  inline void set1(bv32 &vect, bv8 idx)
  {
    assert(idx < 32);
    vect |= (BV32(1) << idx);
  }

  inline void set(bv32 &vect, bv8 idx, bool val)
  {
    assert(idx < 32);
    vect ^= (vect & (BV32(1) << idx)) ^ (static_cast<bv32>(val) << idx);
  }

  inline void invert(bv32 &vect, bv8 idx)
  {
    assert(idx < 32);
    vect ^= (BV32(1) << idx);
  }

  inline bv32 inversion(bv32 vect, bv8 len)
  {
    assert(len < 32);
    return ~vect & max32(len);
  }

  inline bv32 sum(bv32 vect1, bv32 vect2)
  {
    return vect1 ^ vect2;
  }

  inline void add(bv32 &vect, bv32 vect2)
  {
    vect ^= vect2;
  }

  inline bool product(bv32 vect1, bv32 vect2)
  {
    return odd(vect1 & vect2);
  }

  // zeros have the same leads
  inline bool sameLeads(bv32 vect1, bv32 vect2)
  {
    auto vect = vect1 ^ vect2;
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv32 vect, bv32 mask)
  {
    auto masked = vect & mask;
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv32 vect, bv32 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool zero(bv32 vect)
  {
    return vect == BV32(0);
  }
}
