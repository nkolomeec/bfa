#pragma once

#include "bv.h"
#include "vect/iterproxy.h"
#include "vect/iterrevproxy.h"
#include "vect/itermaskproxy.h"

namespace bf
{
  inline IterProxy<bv8> brange(bv8 minimum, bv8 maximum)
  {
    return IterProxy<bv8>(minimum, maximum, minimum > maximum);
  }

  inline IterProxy<bv32> brange(bv32 minimum, bv32 maximum)
  {
    return IterProxy<bv32>(minimum, maximum, minimum > maximum);
  }

  inline IterProxy<bv64> brange(bv64 minimum, bv64 maximum)
  {
    return IterProxy<bv64>(minimum, maximum, minimum > maximum);
  }

  // zero vector is always zero-length vector
  inline IterProxy<bv32> brange(int length)
  {
    assert(0 <= length && length <= 32);
    
    return IterProxy<bv32>(BV32(1) << length, false);
  }

  inline IterProxy<bv8> brange8(int length)
  {
    assert(length >= 0 && length <= 8);
    return IterProxy<bv8>((bv8)(1 << length), false);
  }

  inline IterProxy<bv64> brange64(int length)
  {
    assert(length >= 0 && length <= 64);
    return IterProxy<bv64>(BV64(1) << length, false);
  }

  inline IterMaskProxy<bv32> bmaskrange(bv32 mask, bool skipZero = false)
  {
    return IterMaskProxy<bv32>(mask, skipZero);
  }

  inline IterMaskProxy<bv64> bmaskrange(bv64 mask, bool skipZero = false)
  {
    return IterMaskProxy<bv64>(mask, skipZero);
  }

  inline IterMaskProxy<bv8> bmaskrange(bv8 mask, bool skipZero = false)
  {
    return IterMaskProxy<bv8>(mask, skipZero);
  }
}
