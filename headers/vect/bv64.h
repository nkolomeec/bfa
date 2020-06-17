#pragma once

#include <cassert>

#include "bfa.h"
#include "vect/bvdata.h"
#include "vect/bv32.h"

namespace bf
{
  inline bv64 max64(bv8 n)
  {
    assert(n <= 64);
    return ~((BV64(0xFFFFFFFFFFFFFFFF) << (n & BV64(63))) & (static_cast<bv64>(n >> 6) - 1));
  }

  inline bv64 basis64(bv8 i)
  {
    assert(i < 64);
    return (BV64(1) << i);
  }

  inline bv8 lead(bv64 vect)
  {
    bv8 result = BV8(0);

    bv64 tmp = vect >> 32;
    if (tmp != 0) { vect = tmp; result += BV8(32); }

    return result + lead(static_cast<bv32>(vect));
  }

  inline bv8 size(bv64 vect)
  {
    return lead(vect) + 1;
  }

  inline bv8 weight(bv64 vect)
  {
    return weight(static_cast<bv32>(vect)) + weight(static_cast<bv32>(vect >> 32));
  }

  inline bool odd(bv64 vect)
  {
    return odd(static_cast<bv32>(vect ^ (vect >> 32)));
  }

  inline bool get(bv64 vect, bv8 idx)
  {
    assert(idx < 64);
    return (bool)((vect >> idx) & BV64(1));
  }

  inline void set0(bv64 &vect, bv8 idx)
  {
    assert(idx < 64);
    vect &= ~(BV64(1) << idx);
  }

  inline void set1(bv64 &vect, bv8 idx)
  {
    assert(idx < 64);
    vect |= (BV64(1) << idx);
  }

  inline void set(bv64 &vect, bv8 idx, bool val)
  {
    assert(idx < 64);
    vect ^= (vect & (BV64(1) << idx)) ^ (static_cast<bv64>(val) << idx);
  }

  inline void invert(bv64 &vect, bv8 idx)
  {
    assert(idx < 64);
    vect ^= (BV64(1) << idx);
  }

  inline bv64 inversion(bv64 vect, bv8 len)
  {
    assert(len < 64);
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

  inline bool zero(bv64 vect)
  {
    return vect == BV64(0);
  }
}
