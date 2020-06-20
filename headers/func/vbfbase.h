#pragma once

#include "vect/chunkvector.h"
#include "func/bfbase.h"

#include <vector>

namespace bf
{
  class VBFBase
  {
  protected:
    ChunkVector _values;
    bv8 _n;
    bv8 _m;

    // constructors
  public:
    inline VBFBase()
      :
      VBFBase(0, 1)
    {
    }

    inline VBFBase(int n, int m)
      :
      _n((bv8)n),
      _m((bv8)m),
      _values(m * bf::basis64(n))
    {
      assert(n >= 0 && n <= 32);
      assert(m > 0 && m <= 32);
    }

    // general functions 
  public:
    inline int n() const noexcept
    {
      return _n;
    }

    inline int m() const noexcept
    {
      return _m;
    }
    inline bv64 size() const noexcept
    {
      return basis64(_n);
    }

    inline Domain<bv32> dom() const noexcept
    {
      return Domain<bv32>(_n);
    }

    inline bv32 get(bv32 x) const noexcept
    {
      assert(x < basis64(_n));

      return _values.get(static_cast<bv64>(x) * _m, _m);
    }

    inline void set(bv32 x, bv32 val) noexcept
    {
      assert(x < basis64(_n));
      assert(val < basis64(_m));

      _values.set(static_cast<bv64>(x) * _m, val, _m);
    }

    inline void set(bv32 val) noexcept
    {
      assert(val < basis64(_m));

      for (auto x : dom())
      {
        _values.set(static_cast<bv64>(x) * _m, val, _m);
      }
    }

    std::vector<BFBase> split() const;

    static VBFBase join(const std::vector<BFBase> &coordinates);

  public:
    VBFBase MobiusTransform() const;
  };
}