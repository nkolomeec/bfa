#pragma once

#include "vect/chunkvector.h"
#include "vect/enum.h"

namespace bf
{
  class BFBase
  {
  protected:
    ChunkVector _values;
    bv8 _n;

  protected:
    void resize(bv8 n, bool copy);

    // constructors
  public:
    inline BFBase()
    :
      BFBase(BV8(0))
    {
    }

    inline BFBase(bv8 n)
      :
      _n(n),
      _values(bf::basis64(n))
    {
    }

    // general functions 
  public:
    inline bv8 n() const noexcept
    {
      return _n;
    }

    inline bv64 size() const noexcept
    {
      return basis64(_n);
    }

    inline IterProxy<bv32> dom() const noexcept
    {
      return brange(_n);
    }

    inline bool get(bv32 x) const noexcept
    {
      assert(x < basis64(_n));

      return _values.get(x);
    }

    inline void set(bv32 x, bool val) noexcept
    {
      assert(x < basis64(_n));

      return _values.set(x, val);
    }

    inline void invert(bv32 x) noexcept
    {
      assert(x < basis64(_n));

      return _values.invert(x);
    }

    inline void set(bool val) noexcept
    {
      _values.set(val);

      _values[0] &= (bv64)((static_cast<bv64>(_n < BV8(6)) << (basis64(_n) & BV64(63))) - 1);
    }

  public:
    BFBase MobiusTransform() const;
  };
}