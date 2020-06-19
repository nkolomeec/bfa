#pragma once

#include "vect/chunkvector.h"
#include "vbfbase.h"
#include "anf.h"

#include <vector>
#include <string>

namespace bf
{
  class VBF;

  class VAnf : public VBFBase
  {
  public:
    inline VAnf() = default;

    inline VAnf(int n, int m)
      :
      VBFBase(n, m)
    {
    }

    inline VAnf(const VBFBase &bf)
      :
      VBFBase(bf)
    {
    }

    inline VAnf(VBFBase &&bf)
      :
      VBFBase(std::move(bf))
    {
    }

    VAnf(const VBF&);

    bool setFormula(std::string formula);

    std::ostream& write(std::ostream &stream, bool is0bCoefficients, const char *variableSymbol, unsigned int startVar, const char *startIdx, const char *endIdx, const char *multiplySymbol, const char *sumSymbol) const;

    inline bool coordinate(int i, bv32 x) const
    {
      assert(i >= 0 && i < m());

      return bf::get(get(x), i);
    }

    inline Anf coordinate(int i) const
    {
      assert(i >= 0 && i < m());

      Anf result(n());

      for (auto x : result.dom())
      {
        result.set(x, coordinate(i, x));
      }

      return result;
    }

    inline Anf component(bv32 comp) const
    {
      Anf result(n());

      for (auto x : result.dom())
      {
        result.set(x, bf::product(comp, x));
      }

      return result;
    }

  public:
    int deg() const;
  };

  std::ostream& operator<<(std::ostream& stream, const VAnf &anf);
}