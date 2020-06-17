#include "func/bfbase.h"

namespace
{
  constexpr std::array<bf::bv64, 6> MobiusTemplates = {
    ~BV64(0x5555555555555555),
    ~BV64(0x3333333333333333),
    ~BV64(0x0F0F0F0F0F0F0F0F),
    ~BV64(0x00FF00FF00FF00FF),
    ~BV64(0x0000FFFF0000FFFF),
    ~BV64(0x00000000FFFFFFFF)
  };
}

namespace bf 
{
  void BFBase::resize(bv8 n, bool copy)
  {
    _values.resize(basis64(n));

    if (n <= _n || !copy)
    {
      _n = n;
      return;
    }

    for (; _n < BV8(6) && _n < n; ++_n)
    {
      _values[0] ^= _values[0] << basis8(_n);
    }

    for (; _n < n; ++_n)
    {
      std::memcpy(&_values[0], &_values[basis64(_n) >> 6], basis64(_n) >> 3);
    }
  }

  BFBase BFBase::MobiusTransform() const
  {
    BFBase mobiusF(*this);
    bv32 nChunkSteps = (_n < 6) ? _n : 6;
    auto size = mobiusF._values.size();

    for (bv32 j = 0; j < nChunkSteps; ++j) 
    {
      auto templ = MobiusTemplates[j];
      auto shift = basis64(j);
      
      for (bv64 i = 0; i < size; ++i) 
      {
        auto &val = mobiusF._values[i];
        val ^= (val << shift) & templ;
      }
    }

    for (auto j = nChunkSteps; j < _n; ++j) 
    {
      auto templ = basis64(j - nChunkSteps);

      for (bv64 i = 0; i < size; i += (templ << 1))
      {
        for (auto k = i; k < i + templ; ++k)
        {
          mobiusF._values[k + templ] ^= mobiusF._values[k];
        }
      }
    }

    return mobiusF;
  }
}
