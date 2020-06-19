#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"
#include "vect/bv32.h"

namespace bf
{
  inline bv64 max64(int n)
  {
    assert(n >= 0 && n <= 64);

    return ~((BV64(0xFFFFFFFFFFFFFFFF) << (n & BV64(63))) & (static_cast<bv64>(n >> 6) - 1));
  }

  inline bv64 basis64(int i)
  {
    assert(i >= 0 && i < 64);
    
    return (BV64(1) << i);
  }

  inline bool zero(bv64 vect)
  {
    return vect == BV64(0);
  }

  inline int lead(bv64 vect)
  {
    if (zero(vect))
    {
      return -1;
    }

    auto result = 0;

    bv64 tmp = vect >> 32;
    if (tmp != 0) { vect = tmp; result += 32; }

    tmp = vect >> 16;
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

  inline int size(bv64 vect)
  {
    return lead(vect) + 1;
  }

  inline int size1(bv64 vect)
  {
    return std::max(size(vect), 1);
  }

  inline int weight(bv64 vect)
  {
    return weight(static_cast<bv32>(vect)) + weight(static_cast<bv32>(vect >> 32));
  }

  inline bool get(bv64 vect, int idx)
  {
    assert(idx >= 0 && idx < 64);

    return (bool)((vect >> idx) & BV64(1));
  }

  inline void set0(bv64 &vect, int idx)
  {
    assert(idx >= 0 && idx < 64);

    vect &= ~(BV64(1) << idx);
  }

  inline void set1(bv64 &vect, int idx)
  {
    assert(idx >= 0 && idx < 64);

    vect |= (BV64(1) << idx);
  }

  inline void set(bv64 &vect, int idx, bool val)
  {
    assert(idx >= 0 && idx < 64);

    vect ^= (vect & (BV64(1) << idx)) ^ (static_cast<bv64>(val) << idx);
  }

  inline void invert(bv64 &vect, int idx)
  {
    assert(idx >= 0 && idx < 64);

    vect ^= (BV64(1) << idx);
  }

  inline bv64 inversion(bv64 vect, int len)
  {
    assert(len >= 0 && len < 64);
    return ~vect & max64(len);
  }

  inline bv64 sum(bv64 vect1, bv64 vect2)
  {
    return vect1 ^ vect2;
  }

  inline void add(bv64 &vect, bv64 vect2)
  {
    vect ^= vect2;
  }

  inline bool odd(bv64 vect)
  {
    vect ^= vect >> 32;
    vect ^= vect >> 16;
    vect ^= vect >> 8;
    vect ^= vect >> 4;
    vect ^= vect >> 2;
    vect ^= vect >> 1;

    return get(vect, 0);
  }

  inline bool product(bv64 vect1, bv64 vect2)
  {
    return odd(vect1 & vect2);
  }

  inline bool sameLeads(bv64 vect1, bv64 vect2)
  {
    auto vect = vect1 ^ vect2;
    return vect <= vect1 && vect <= vect2;
  }

  // zero has "lead" in any mask
  inline bool containsLead(bv64 vect, bv64 mask)
  {
    auto masked = vect & mask;
    return (masked ^ vect) <= masked;
  }

  inline bool satisfy(bv64 vect, bv64 mask)
  {
    return (vect & mask) == vect;
  }

  inline bool weight1(bv64 vect)
  {
    return !zero(vect) && zero(vect & (vect - 1));
  }
}
