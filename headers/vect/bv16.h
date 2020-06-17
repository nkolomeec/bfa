#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"
#include "vect/bv8.h"

namespace bf
{
  inline bv16 max16(bv8 n)
  {
    assert(n <= 16);
    return static_cast<bv16>(~(UINT64_C(0xFFFF) << n));
  }

  inline bv16 basis16(bv8 i)
  {
    assert(i < 16);
    return (BV16(1) << i);
  }

  inline bv8 lead(bv16 vect)
  {
    bv8 result = BV8(0);
    
    bv16 tmp = vect >> 8;
    if (tmp != 0) { vect = tmp; result += BV8(8); }

    return result + lead(static_cast<bv8>(vect));
  }

  inline bv8 size(bv16 vect)
  {
    return lead(vect) + 1;
  }

  inline bv8 weight(bv16 vect)
  {
    return _weightTable[BV8(0xFF) & vect] + _weightTable[vect >> 8];
  }

  inline bool odd(bv16 vect)
  {
    vect ^= vect >> 8;
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return (vect & BV16(1)) != 0;
  }

  inline bool get(bv16 vect, bv8 idx)
  {
    assert(idx < 16);
    return (bool)((vect >> idx) & BV16(1));
  }


  inline void set0(bv16 &vect, bv8 idx)
  {
    assert(idx < 16);
    vect &= ~(BV16(1) << idx);
  }

  inline void set1(bv16 &vect, bv8 idx)
  {
    assert(idx < 16);
    vect |= (BV16(1) << idx);
  }

  inline void set(bv16 &vect, bv8 idx, bool val)
  {
    assert(idx < 16);
    vect ^= (vect & (BV16(1) << idx)) ^ (static_cast<bv16>(val) << idx);
  }

  inline void invert(bv16 &vect, bv8 idx)
  {
    assert(idx < 16);
    vect ^= (BV16(1) << idx);
  }

  inline bv16 inversion(bv16 vect, bv8 len)
  {
    assert(len < 16);
    return ~vect & max16(len);
  }

  inline bv16 sum(bv16 vect1, bv16 vect2)
  {
    return vect1 ^ vect2;
  }

  inline void add(bv16 &vect, bv16 vect2)
  {
    vect ^= vect2;
  }

  inline bool product(bv16 vect1, bv16 vect2)
  {
    return odd((bv16)(vect1 & vect2));
  }

  // zeros have the same leads
  inline bool sameLeads(bv16 vect1, bv16 vect2)
  {
    auto vect = vect1 ^ vect2;
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv16 vect, bv16 mask)
  {
    auto masked = vect & mask;
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv16 vect, bv16 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool zero(bv16 vect)
  {
    return vect == BV16(0);
  }
}
