#include "func/bf.h"
#include "func/anf.h"
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

  std::ostream& operator<<(std::ostream &stream, const BF &bf)
  {
    for (auto x : bf.dom())
    {
      stream << bf.get(x);
    }

    return stream;
  }
}
