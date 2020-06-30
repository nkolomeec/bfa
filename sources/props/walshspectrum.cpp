#include "func/bf.h"
#include "props/walshspectrum.h"
#include "vect/bvio.h"


namespace bf
{
  WalshSpectrum::WalshSpectrum(const BF &f)
    :
    _walsh(bf::basis64(f.n())),
    _n((bv8)f.n())
  {
    for (auto x : f.dom())
    {
      _walsh[x] = (f.get(x) ? -1 : 1);
    }

    MultiplyHn(_walsh, _n);
  }

  bool WalshSpectrum::function(BF &f) const
  {
    BF func(_n);
    auto values(_walsh);

    MultiplyHn(values, _n);

    auto expn = size();

    for (auto x : func.dom())
    {
      if (std::abs(values[x]) != expn)
      {
        return false;
      }

      func.set(x, values[x] < 0);
    }

    f.swap(func);

    return true;
  }

  void WalshSpectrum::sign(BF& f, bool valueOnZero) const
  {
    BF func(_n);

    for (auto x : func.dom())
    {
      auto c = _walsh[x];
      func.set(x, (c < 0) ^ (valueOnZero && c == 0));
    }

    f.swap(func);
  }

  std::vector<int64_t> WalshSpectrum::correlation(const WalshSpectrum &wg) const
  {
    auto n = std::max(this->_n, wg._n);
    auto multiplier = std::max(bf::basis64(n - _n), bf::basis64(n - wg._n));
    auto minSize = std::min(this->size(), wg.size());
    auto size = bf::basis64(n);

    std::vector<int64_t> values(size);

    for (bv64 i = 0; i < minSize; ++i)
    {
      values[i] = multiplier * _walsh[i] * wg._walsh[i];
    }

    for (bv64 i = minSize; i < size; ++i)
    {
      values[i] = 0;
    }

    MultiplyHn(values, n);

    for (bv64 i = 0; i < size; ++i)
    {
      values[i] = values[i] >= 0 ? (int64_t)(static_cast<uint64_t>(abs(values[i])) >> n) : -(int64_t)(static_cast<uint64_t>(abs(values[i])) >> n);
    }

    return values;
  }

  std::vector<int64_t> WalshSpectrum::autocorrelation() const
  {
    return correlation(*this);
  }

  uint64_t WalshSpectrum::radius() const
  {
    uint64_t max = 0;

    for (auto x : _walsh)
    {
      auto a = (uint64_t)abs(x);
      
      if (a > max)
      {
        max = a;
      }
    }

    return max;
  }

  uint32_t WalshSpectrum::nonlinearity() const
  {
    if (_n < BV8(2))
    {
      return 0;
    }

    return (uint32_t)(basis64(_n - 1) - (radius() >> 1));
  }

  int WalshSpectrum::ci() const
  {
    int immunity = _n;

    for (bv64 i = 1; i < size(); ++i)
    {
      if (_walsh[i] != 0)
      {
        auto w = bf::weight(i);

        if (w <= immunity)
        {
          immunity = w - 1;

          if (immunity == 0)
          {
            return immunity;
          }
        }
      }
    }

    return immunity;
  }

  int WalshSpectrum::resilency() const
  {
    if (!balanced())
    {
      return 0;
    }

    return ci();
  }

  bool WalshSpectrum::balanced() const
  {
    return _walsh[0] == 0;
  }

  void WalshSpectrum::MultiplyHn(std::vector<int64_t> &data, int n)
  {
    auto size = bf::basis64(n);

    for (auto j = 0; j < n; ++j)
    {
      auto templ = basis64(j);

      for (bv64 i = 0; i < size; i += (templ << 1))
      {
        for (auto k = i; k < i + templ; ++k)
        {
          auto x = data[k];
          auto &y = data[k + templ];

          data[k] += y;
          y = x - y;
        }
      }
    }
  }
}
