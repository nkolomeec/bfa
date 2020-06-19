#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"

namespace bf
{
  inline bv16 max16(int n)
  {
    assert(n >= 0 && n <= 16);

    return static_cast<bv16>(~(UINT64_C(0xFFFF) << n));
  }

  inline bv16 basis16(int i)
  {
    assert(i >= 0 && i < 16);

    return (BV16(1) << i);
  }

  inline bool zero(bv16 vect)
  {
    return vect == BV16(0);
  }

  inline int lead(bv16 vect)
  {
    if (zero(vect))
    {
      return -1;
    }

    auto result = 0;

    bv16 tmp = vect >> 8;
    if (tmp != 0) { vect = tmp; result += 8; }

    tmp = vect >> 4;
    if (tmp != 0) { vect = tmp; result += 4; }

    tmp = vect >> 2;
    if (tmp != 0) { vect = tmp; result += 2; }

    tmp = vect >> 1;
    if (tmp != 0) { result += 1; }

    return result;
  }

  inline int size(bv16 vect)
  {
    return lead(vect) + 1;
  }

  inline int size1(bv16 vect)
  {
    return std::max(size(vect), 1);
  }

  inline int weight(bv16 vect)
  {
    return _weightTable[BV16(0xFF) & vect] + _weightTable[vect >> 8];
  }

  inline bool get(bv16 vect, int idx)
  {
    assert(idx >= 0 && idx < 16);

    return (bool)((vect >> idx) & 1);
  }


  inline void set0(bv16 &vect, int idx)
  {
    assert(idx >= 0 && idx < 16);

    vect &= ~(1 << idx);
  }

  inline void set1(bv16 &vect, int idx)
  {
    assert(idx >= 0 && idx < 16);
    
    vect |= (1 << idx);
  }

  inline void set(bv16 &vect, int idx, bool val)
  {
    assert(idx >= 0 && idx < 16);

    vect ^= (vect & (1 << idx)) ^ (static_cast<int>(val) << idx);
  }

  inline void invert(bv16 &vect, int idx)
  {
    assert(idx >= 0 && idx < 16);

    vect ^= (1 << idx);
  }

  inline bv16 inversion(bv16 vect, int len)
  {
    assert(len >= 0 && len <= 16);
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

  inline bool odd(bv16 vect)
  {
    vect ^= vect >> 8;
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return get(vect, 0);
  }

  inline bool product(bv16 vect1, bv16 vect2)
  {
    return odd((bv16)(vect1 & vect2));
  }

  // zeros have the same leads
  inline bool sameLeads(bv16 vect1, bv16 vect2)
  {
    bv16 vect = (bv16)(vect1 ^ vect2);
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv16 vect, bv16 mask)
  {
    bv16 masked = (bv16)(vect & mask);
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv16 vect, bv16 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool weight1(bv16 vect)
  {
    return !zero(vect) && zero((bv16)(vect & (vect - 1)));
  }
}
