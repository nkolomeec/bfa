#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"

namespace bf
{
  inline bv8 max8(int n)
  {
    assert(n >= 0 && n <= 8);
    return static_cast<bv8>(~(UINT64_C(0xFF) << n));
  }

  inline bv8 basis8(int i)
  {
    assert(i >= 0 && i < 8);

    return (bv8)(BV8(1) << i);
  }

  inline bool zero(bv8 vect)
  {
    return vect == BV8(0);
  }

  inline int lead(bv8 vect)
  {
    if (zero(vect))
    {
      return -1;
    }

    auto result = 0;

    bv8 tmp = vect >> 4;
    if (tmp != 0) { vect = tmp; result += 4; }

    tmp = vect >> 2;
    if (tmp != 0) { vect = tmp; result += 2; }

    tmp = vect >> 1;
    if (tmp != 0) { result += 1; }

    return result;
  }

  inline int size(bv8 vect)
  {
    return lead(vect) + 1;
  }

  inline int size1(bv8 vect)
  {
    return std::max(size(vect), 1);
  }

  inline int weight(bv8 vect)
  {
    return _weightTable[vect];
  }

  inline bool get(bv8 vect, int idx)
  {
    assert(idx >= 0 && idx < 8);

    return (bool)((vect >> idx) & 1);
  }

  inline void set0(bv8 &vect, int idx)
  {
    assert(idx >= 0 && idx < 8);

    vect &= ~(1 << idx);
  }

  inline void set1(bv8 &vect, int idx)
  {
    assert(idx >= 0 && idx < 8);

    vect |= (1 << idx);
  }

  inline void set(bv8 &vect, int idx, bool val)
  {
    assert(idx >= 0 && idx < 8);

    vect ^= (vect & (1 << idx)) ^ (static_cast<int>(val) << idx);
  }
    
  inline void invert(bv8 &vect, int idx)
  {
    assert(idx >= 0 && idx < 8);

    vect ^= (BV8(1) << idx);
  }

  inline bv8 inversion(bv8 vect, int len)
  {
    assert(len >= 0 && len <= 8);

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

  inline bool odd(bv8 vect)
  {
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return get(vect, 0);
  }

  inline bool product(bv8 vect1, bv8 vect2)
  {
    return odd((bv8)(vect1 & vect2));
  }

  // zeros have the same leads
  inline bool sameLeads(bv8 vect1, bv8 vect2)
  {
    auto vect = (bv8)(vect1 ^ vect2);
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv8 vect, bv8 mask)
  {
    auto masked = (bv8)(vect & mask);
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv8 vect, bv8 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool weight1(bv8 vect)
  {
    return !zero(vect) && zero((bv8)(vect & (vect - 1)));
  }

}
