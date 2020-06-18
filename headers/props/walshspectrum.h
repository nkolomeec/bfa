#pragma once

#include "bv.h"
#include "vect/iterproxy.h"

#include <vector>
#include <cstdint>

namespace bf
{
  class BF;

  class WalshSpectrum
  {
  public:
    WalshSpectrum()
      :
      _walsh(1, -1),
      _n(0)
    {
    }

    explicit WalshSpectrum(const BF &f);

    bool function(BF &f);

    std::vector<int64_t> correlation(const WalshSpectrum &wg) const;

    std::vector<int64_t> autocorrelation() const;

    uint64_t size() const
    {
      return _walsh.size();
    }

    int64_t& operator[](bv64 x)
    {
      assert(x < size());
      return _walsh[x];
    }

    int64_t operator[](bv64 x) const
    {
      assert(x < size());
      return _walsh[x];
    }

    uint64_t radius() const;

    uint32_t nonlinearity() const;

    bv8 ci() const;

    bv8 resilency() const;

    bool balanced() const;

    bool bent() const
    {
      return !bf::get(_n, 0) && radius() == basis64(_n >> 1);
    }

    bv8 n()
    {
      return _n;
    }

    IterProxy<bv32> dom() const
    {
      return brange(_n);
    }

  private:
    static void MultiplyHn(std::vector<int64_t> &data, bv8 n);

  private:
    bv8 _n;
    std::vector<int64_t> _walsh;
  };
}