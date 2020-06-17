#pragma once

#include "bfbase.h"

#include <istream>
#include <ostream>

namespace bf
{
  class BF;

  class Anf : public BFBase
  {
  public:
    inline Anf() = default;

    inline Anf(bv8 n)
      :
      BFBase(n)
    {
    }

    inline Anf(bool value)
      :
      Anf()
    {
      set(BV32(0), value);
    }

    inline Anf(const BFBase &bf)
      :
      BFBase(bf)
    {
    }

    inline Anf(BFBase &&bf)
      :
      BFBase(std::move(bf))
    {
    }

    Anf(const BF&);

  public:
    int32_t deg() const;

    Anf minAnnihilator(bv8 upToDegree) const;

    inline void resize(bv8 n)
    {
      BFBase::resize(n, false);
    }

    bool setFormula(const std::string &formula);

    std::ostream& write(std::ostream &stream, const char *variableSymbol, unsigned int startVar, const char *startIdx, const char *endIdx, const char *multiplySymbol, const char *sumSymbol) const;
  };

  // std::istream& operator>>(std::istream& stream, Anf &oanf);
  std::ostream& operator<<(std::ostream &stream, const Anf &anf);
}

