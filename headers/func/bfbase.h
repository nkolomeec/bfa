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
    void resize(int n, bool copy);

    // constructors
  public:
    inline BFBase()
    :
      BFBase(0)
    {
    }

    inline BFBase(int n)
      :
      _n((bv8)n),
      _values(bf::basis64(n))
    {
      assert(n >= 0 && n <= 32);
    }

    // general functions 
  public:
    inline int n() const noexcept
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

      _values[0] &= (bv64)((static_cast<bv64>(_n < 6) << (basis64(_n) & BV64(63))) - 1);
    }

  public:
    BFBase MobiusTransform() const;
  };
}