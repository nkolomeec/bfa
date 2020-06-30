#include "func/bf.h"
#include "func/anf.h"
#include "vect/domainmask.h"
#include "vect/linearcombinations.h"
#include "props/walshspectrum.h"

namespace bf 
{
  BF::BF(const Anf &anf)
    :
    BFBase(anf.MobiusTransform())
  {
  }

  int64_t BF::Walsh(bv32 x) const
  {
    int64_t result = 0;

    for (auto y : dom())
    {
      result += power(get(y) ^ bf::product(x, y));
    }

    return result;
  }

  int64_t BF::correlation(const BF &g, bv32 x) const
  {
    int64_t result = 0;
    auto max = _n < g._n ? bf::max32(g._n) : bf::max32(_n);

    for (auto y = BV32(0); y <= max; ++y)
    {
      result += power(getSafe(y) ^ g.getSafe(y ^ x));
    }

    return result;
  }

  bool BF::affine(bv32 &coeff, bool &c) const
  {
    auto w = BV32(0);
    auto step1 = std::min(n(), 6);

    for (auto i = 0; i < step1; ++i)
    {
      auto len = basis64(i);
      bv64 max = (BV64(1) << len) - 1;
      
      auto val = (_values[0] ^ (_values[0] >> len)) & max;

      if (val != BV64(0) && val != max)
      {
        return false;
      }

      bf::set(w, i, val != BV64(0));
    }

    for (auto i = step1; i < _n; ++i)
    {
      auto shift = basis64(i - step1);
      auto diff = _values[0] ^ _values[shift];
      
      if (diff != BV64(0) && diff + 1 != BV64(0))
      {
        return false;
      }

      for (auto j = 1; j < shift; ++j)
      {
        if ((_values[j] ^ _values[j + shift]) != diff)
        {
          return false;
        }
      }

      bf::set(w, i, diff != BV64(0));
    }

    coeff = w;
    c = get(BV32(0));

    return true;
  }

  BF BF::subfunction(const bv32* basis, unsigned int count, bv32 coset) const
  {
    assert(count <= 32);

    BF result(count);
    auto safe = bf::max32(_n);

    for (auto& cmb : LinearCombinations<bv32>(coset, basis, count))
    {
      result.set((bv32)cmb.coordinates(), get(cmb.combination() & safe));
    }

    return result;
  }

  BF BF::subfunction(bv32 coordinates, bv32 coset) const
  {
    BF result(bf::weight(coordinates));
    auto safe = bf::max32(_n);
    auto curr = BV32(0);

    for (auto x : DomainMask<bv32>(coordinates))
    {
      result.set(curr, get(x & safe));
      ++curr;
    }

    return result;
  }

  std::ostream& operator<<(std::ostream &stream, const BF &bf)
  {
    for (auto x : bf.dom())
    {
      stream << bf.get(x);
    }

    return stream;
  }
}
