#pragma once

#include "vect/chunkvector.h"
#include "vbfbase.h"

#include <vector>
#include <string>

namespace bf
{
  class VBF;

  class VAnf : public VBFBase
  {
  public:
    inline VAnf() = default;

    inline VAnf(bv8 n, bv8 m)
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

  public:
    int32_t deg() const;
  };

  std::ostream& operator<<(std::ostream& stream, const VAnf &anf);
}