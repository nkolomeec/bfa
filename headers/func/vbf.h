#pragma once

#include "vect/chunkvector.h"
#include "func/vbfbase.h"

#include <vector>

namespace bf
{
  class VAnf;

  class VBF : public VBFBase
  {
  public:
    inline VBF() = default;

    inline VBF(bv8 n, bv8 m)
      :
      VBFBase(n, m)
    {
    }

    inline VBF(const VBFBase &bf)
      :
      VBFBase(bf)
    {
    }

    inline VBF(VBFBase &&bf)
      :
      VBFBase(std::move(bf))
    {
    }

    VBF(const VAnf&);

    VBF& operator+=(const VBF &f)
    {
      assert(_n == f._n && _m == f._m);

      _values ^= f._values;

      return *this;
    }

    VBF& operator*=(const VBF &f)
    {
      assert(_n == f._n && _m == f._m);

      _values &= f._values;

      return *this;
    }
  };
}