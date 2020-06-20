#include "func/vbf.h"
#include "func/vanf.h"

#include "formula/lexem.h"
#include "formula/parsedcalculator.h"
#include "integer/rational.h"
#include "iter/cnkiter.h"
#include "vect/bvio.h"

#include <sstream>

namespace
{
  struct VAnfParsedCalculator : public lexem::ParsedCalculator<bf::VBF>
  {
    VAnfParsedCalculator(const ParsedFormula &f)
      :
      ParsedCalculator<bf::VBF>(f)
    {
    }

    virtual void plus(bf::VBF &a, const bf::VBF& b)
    {
      a += b;
    }

    virtual void minus(bf::VBF& a, const bf::VBF& b)
    {
      plus(a, b);
    }

    virtual void minus(bf::VBF& a)
    {
    }

    virtual void mult(bf::VBF& a, const bf::VBF& b)
    {
      a *= b;
    }

    virtual bf::VBF convertConstant(bf::bv64 a)
    {
      bf::VBF result(nVariables, bf::size1(maxConstant));
      
      result.set((bf::bv32)a);

      return result;
    }

    virtual bf::VBF convertVariable(unsigned int globalId)
    {
      bf::VBF result(nVariables, bf::size1(maxConstant));

      auto val = bf::max32(result.m());

      for (auto x : result.dom())
      {
        if (bf::get(x, (bf::bv8)globalId))
        {
          result.set(x, val);
        }
      }

      return result;
    }
  };
}

namespace bf 
{
  VAnf::VAnf(const VBF &bf)
    :
    VBFBase(bf.MobiusTransform())
  {
  }

  int VAnf::deg() const
  {
    int deg = -1;
    for (auto x : dom())
    {
      if (!bf::zero(get(x)))
      {
        auto wt = bf::weight(x);

        if (wt > deg)
        {
          deg = wt;
        }
      }
    }

    return deg;
  }

  bool VAnf::setFormula(std::string formula)
  {
    std::istringstream sstream(formula);

    auto parsed = lexem::ParseFormula(sstream, {});

    if (!parsed.isOk() || !sstream.eof())
    {
      return false;
    }

    if (parsed.hasRationalContext || parsed.endWithSpecial || 
      bf::size(parsed.maxConstant) > BV8(32) || parsed.nVariables > (unsigned int)32)
    {
      return false;
    }

    VAnfParsedCalculator calc(parsed);

    if (!calc.calculate())
    {
      return false;
    }

    *this = VAnf(calc.value);
    
    return true;
  }

  std::ostream& VAnf::write(std::ostream &stream, bool is0bCoefficients, const char *variableSymbol, unsigned int startVar, const char *startIdx, const char *endIdx, const char *multiplySymbol, const char *sumSymbol) const
  {
    auto i = n();
    uint64_t nMonoms = 0;

    auto max = bf::max32(m());

    while (i > 0) {
      for (auto j = CnkIter(n(), i); j; ++j)
      {
        auto val = get(j.first32());
        if (val != 0)
        {
          if (nMonoms > 0)
            stream << sumSymbol;

            if (is0bCoefficients)
            {
              stream << "0b" << bf::bv(val);
            }
            else
            {
              stream << val;
            }

          for (auto k = 0; k < i; ++k)
          {
            stream << variableSymbol << startIdx << j[k] + startVar << endIdx;
          }
          nMonoms++;
        }
      }
      i--;
    }

    auto val0 = get(0);

    if (val0 != 0)
    {
      stream << (nMonoms > 0 ? sumSymbol : "");
      
      if (is0bCoefficients)
      {
        stream << bf::bv(val0);
      }
      else
      {
        stream << val0;
      }
    }
    else if (nMonoms == 0)
    {
      stream << '0';
    }

    return stream;
  }

  std::ostream& operator<<(std::ostream& stream, const VAnf &anf)
  {
    return anf.write(stream, false, "x", 1, "", "", "", " + ");
  }
}
