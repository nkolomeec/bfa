#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"

namespace bf
{
  inline bv32 max32(int n)
  {
    assert(n >= 0 && n <= 32);
    return static_cast<bv32>(~(UINT64_C(0xFFFFFFFF) << n));
  }

  inline bv32 basis32(int i)
  {
    assert(i >= 0 && i < 32);
    return (BV32(1) << i);
  }

  inline bool zero(bv32 vect)
  {
    return vect == BV32(0);
  }

  inline int lead(bv32 vect)
  {
    if (zero(vect))
    {
      return -1;
    }

    auto result = 0;

    auto tmp = vect >> 16;
    if (tmp != 0) { vect = tmp; result += 16; }

    tmp = vect >> 8;
    if (tmp != 0) { vect = tmp; result += 8; }

    tmp = vect >> 4;
    if (tmp != 0) { vect = tmp; result += 4; }

    tmp = vect >> 2;
    if (tmp != 0) { vect = tmp; result += 2; }

    tmp = vect >> 1;
    if (tmp != 0) { result += 1; }

    return result;
  }

  inline int size(bv32 vect)
  {
    return lead(vect) + 1;
  }

  inline int size1(bv32 vect)
  {
    return std::max(size(vect), 1);
  }

  inline int weight(bv32 vect)
  {
    return _weightTable[BV32(0xFF) & vect] +
      _weightTable[BV32(0xFF) & (vect >> 8)] +
      _weightTable[BV32(0xFF) & (vect >> 16)] +
      _weightTable[BV32(0xFF) & (vect >> 24)];
  }

  inline bool get(bv32 vect, int idx)
  {
    assert(idx >= 0 && idx < 32);

    return (bool)((vect >> idx) & BV32(1));
  }

  inline void set0(bv32 &vect, int idx)
  {
    assert(idx >= 0 && idx < 32);

    vect &= ~(BV32(1) << idx);
  }

  inline void set1(bv32 &vect, int idx)
  {
    assert(idx >= 0 && idx < 32);

    vect |= (BV32(1) << idx);
  }

  inline void set(bv32 &vect, int idx, bool val)
  {
    assert(idx >= 0 && idx < 32);

    vect ^= (vect & (BV32(1) << idx)) ^ (static_cast<bv32>(val) << idx);
  }

  inline void invert(bv32 &vect, int idx)
  {
    assert(idx >= 0 && idx < 32);

    vect ^= (BV32(1) << idx);
  }

  inline bv32 inversion(bv32 vect, int len)
  {
    assert(len >= 0 && len <= 32);

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

  inline bool odd(bv32 vect)
  {
    vect ^= vect >> 16;
    vect ^= vect >> 8;
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return get(vect, 0);
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

  inline bool weight1(bv32 vect)
  {
    return !zero(vect) && zero(vect & (vect - 1));
  }

  inline bv64 leastOne(bv32 vect)
  {
    return vect & (bv32)(-(int32_t)vect);
  }
}
