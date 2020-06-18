#pragma once

#include "vect/chunkvector.h"
#include "func/bf.h"
#include "func/vbfbase.h"

#include <vector>

namespace bf
{
  class VAnf;
  class FPolynomial;

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
    
    VBF(const FPolynomial&);

    inline VBF& operator+=(const VBF &f)
    {
      assert(_n == f._n && _m == f._m);

      _values ^= f._values;

      return *this;
    }

    inline VBF& operator*=(const VBF &f)
    {
      assert(_n == f._n && _m == f._m);

      _values &= f._values;

      return *this;
    }

    inline BF component(bv32 comp) const
    {
      BF result(n());

      for (auto x : result.dom())
      {
        result.set(x, component(comp, x));
      }

      return result;
    }

    inline bool component(bv32 comp, bv32 x) const
    {
      return bf::product(comp, get(x));
    }

    inline bool coordinate(bv8 i, bv32 x) const
    {
      assert(i < m());

      return bf::get(get(x), i);
    }

    inline BF coordinate(bv8 i) const
    {
      assert(i < m());

      BF result(n());

      for (auto x : result.dom())
      {
        result.set(x, coordinate(i, x));
      }

      return result;
    }

    IterProxy<bv32> components() const
    {
      auto all = brange(_m);
      ++all;
      return all;
    }
  };
}