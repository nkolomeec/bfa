#pragma once

#include "func/bfbase.h"
#include "props/walshspectrum.h"

#include <ostream>
#include <vector>

namespace bf
{
  class Anf;

  class BF : public BFBase
  {
  public:
    inline BF() = default;

    inline BF(bv8 n)
      :
      BFBase(n)
    {
    }

    inline BF(const BFBase &bf)
      :
      BFBase(bf)
    {
    }

    inline BF(BFBase &&bf)
      :
      BFBase(std::move(bf))
    {
    }

    BF(const Anf&);
    
    BF& operator+=(const BF &f)
    {
      assert(_n == f._n);

      _values ^= f._values;

      return *this;
    }

    BF& operator*=(const BF &f)
    {
      assert(_n == f._n);

      _values &= f._values;

      return *this;
    }

    inline void resize(bv8 n)
    {
      BFBase::resize(n, true);
    }

    bool getSafe(bv32 x) const
    {
      return get(x & bf::max32(_n));
    }

    int64_t Walsh(bv32 x) const;

    WalshSpectrum Walsh() const
    {
      return WalshSpectrum(*this);
    }

    int64_t correlation(const BF &g, bv32 x) const;

    int64_t autocorrelation(bv32 x) const
    {
      return correlation(*this, x);
    }

    bv64 weight() const
    {
      auto wt = BV64(0);
      
      for (auto i = BV64(0); i < _values.size(); ++i)
      {
        wt += bf::weight(_values[i]);
      }

      return wt;
    }

    bool balanced() const
    {
      return (weight() << 1) == bf::basis64(n());
    }
  };

  std::ostream& operator<<(std::ostream &stream, const BF &bf);
}

