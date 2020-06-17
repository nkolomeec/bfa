#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"

namespace bf
{
  inline bv8 max8(bv8 n)
  {
    assert(n <= 8);
    return static_cast<bv8>(~(UINT64_C(0xFF) << n));
  }

  inline bv8 basis8(bv8 i)
  {
    assert(i < 8);
    return (BV8(1) << i);
  }

  inline bv8 lead(bv8 vect)
  {
    bv8 result = BV8(0);

    bv8 tmp = vect >> 4;
    if (tmp != 0) { vect = tmp; result += BV8(4); }

    tmp = vect >> 2;
    if (tmp != 0) { vect = tmp; result += BV8(2); }

    tmp = vect >> 1;
    if (tmp != 0) { result += BV8(1); }

    return result;
  }

  inline bv8 size(bv8 vect)
  {
    return lead(vect) + 1;
  }

  inline bv8 weight(bv8 vect)
  {
    return _weightTable[vect];
  }

  inline bool odd(bv8 vect)
  {
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return (bool)(vect & BV8(1));
  }

  inline bool get(bv8 vect, bv8 idx)
  {
    assert(idx < 8);
    return (bool)((vect >> idx) & BV8(1));
  }

  inline void set0(bv8 &vect, bv8 idx)
  {
    assert(idx < 8);
    vect &= ~(BV8(1) << idx);
  }

  inline void set1(bv8 &vect, bv8 idx)
  {
    assert(idx < 8);
    vect |= (BV8(1) << idx);
  }

  inline void set(bv8 &vect, bv8 idx, bool val)
  {
    assert(idx < 8);
    vect ^= (vect & (BV8(1) << idx)) ^ (static_cast<bv8>(val) << idx);
  }
    
  inline void invert(bv8 &vect, bv8 idx)
  {
    assert(idx < 8);
    vect ^= (BV8(1) << idx);
  }

  inline bv8 inversion(bv8 vect, bv8 len)
  {
    assert(len < 8);
    return ~vect & max8(len);
  }

  inline bv8 sum(bv8 vect1, bv8 vect2)
  {
    return vect1 ^ vect2;
  }

  inline void add(bv8 &vect, bv8 vect2)
  {
    vect ^= vect2;
  }

  inline bool product(bv8 vect1, bv8 vect2)
  {
    return odd(vect1 & vect2);
  }

  // zeros have the same leads
  inline bool sameLeads(bv8 vect1, bv8 vect2)
  {
    auto vect = vect1 ^ vect2;
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv8 vect, bv8 mask)
  {
    auto masked = vect & mask;
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv8 vect, bv8 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool zero(bv8 vect)
  {
    return vect == BV8(0);
  }
}
