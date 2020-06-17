#include "func/vbf.h"
#include "func/fpolynomial.h"
#include "formula/lexem.h"
#include "formula/parsedcalculator.h"
#include "gf2/gf2table.h"
#include "gf2/gf2factory.h"
#include "vect/bvio.h"
#include "vect/enum.h"

#include <sstream>
#include <vector>
#include <stack>

#include <iostream>

namespace
{
  struct FPolynomialParsedCalculator : public lexem::ParsedCalculator<bf::VBF>
  {
    std::shared_ptr<const bf::GF2> field;

    FPolynomialParsedCalculator(const ParsedFormula &f, std::shared_ptr<const bf::GF2> gf2)
      :
      ParsedCalculator<bf::VBF>(f),
      field(gf2)
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
      for (auto x : a.dom())
      {
        a.set(x, field->mult(a.get(x), b.get(x)));
      }
    }

    bf::bv32 fieldMax()
    {
      return bf::max32(field->n());
    }

    virtual bool div(bf::VBF& a, const bf::VBF& b)
    {
      for (auto x : b.dom())
      {
        if (bf::zero(b.get(x)))
        {
          return false;
        }
      }

      for (auto x : a.dom())
      {
        a.set(x, field->mult(a.get(x), field->pow(b.get(x), fieldMax() - 1)));
      }

      return true;
    }

    virtual bool pow(bf::VBF& a, const integer::Rational &b)
    {
      if (b.gcd(b.q, fieldMax()) != 1)
      {
        return false;
      }

      auto p = b.mod(b.p, fieldMax()) * b.inversion(b.q, fieldMax());

      if (bf::zero(p))
      {
        p = fieldMax();
      }

      for (auto x : a.dom())
      {
        a.set(x, field->pow(a.get(x), p));
      }

      return true;
    }

    virtual bool function(bf::VBF &a, bf::bv8 functionId)
    {
      if (functionId != BV8(0))
      {
        return false;
      }

      // trace
      for (auto x : a.dom())
      {
        a.set(x, field->tr(a.get(x)));
      }

      return true;
    }

    virtual bf::VBF convertConstant(bf::bv64 a)
    {
      bf::VBF result((bf::bv8)(field->n() * nVariables), field->n());

      result.set(field->mod(a));

      return result;
    }

    virtual bf::VBF convertVariable(unsigned int globalId)
    {
      bf::VBF result((bf::bv8)(field->n() * nVariables), field->n());

      for (auto x : result.dom())
      {
        result.set(x, (x >> (field->n() * globalId) & bf::max32(field->n())));
      }

      return result;
    }
  };

  bf::bv32 getFunctionValue(const bf::VBF& f, bf::bv32 x, bf::bv8 fieldSize)
  {
    return f.get(x & bf::max32(f.n())) & bf::max32(fieldSize);
  }
}

namespace bf 
{
  bool FPolynomial::setFormula(std::string formula)
  {
    std::istringstream sstream(formula);

    auto parsed = lexem::ParseFormula(sstream, { "tr" });

    if (!parsed.isOk() || !sstream.eof())
    {
      return false;
    }

    auto field = parsed.endWithSpecial ? GF2Factory::createMapped(parsed.specialConstant, false) : _field;

    if ((int)field->n() * parsed.nVariables > 32)
    {
      return false;
    }

    if (field == nullptr)
    {
      return false;
    }

    FPolynomialParsedCalculator calc(parsed, field);

    if (!calc.calculate())
    {
      return false;
    }

    *this = FPolynomial(field, (bv8)parsed.nVariables, calc.value);

    return true;
  }

  FPolynomial::FPolynomial(std::shared_ptr<const GF2> gf2, bv8 nPolynomialVars, const VBF &f)
    :
    FPolynomial(gf2, nPolynomialVars)
  {
    auto fieldSize = _field->n();
    auto fieldMax = bf::max32(fieldSize);

    for (auto i : dom())
    {
      auto sum = BV32(0);

      for (auto a : dom())
      {
        auto product = BV32(1);

        for (auto j = BV8(0); j < _nPolynomialVars; ++j)
        {
          auto jDegree = (i >> (j * fieldSize)) & fieldMax;
          auto ja = (a >> (j * fieldSize)) & fieldMax;

          if (zero(jDegree))
          {
            if (zero(ja))
            {
              continue;
            }

            product = 0;
            break;
          }
          
          if (jDegree < fieldMax)
          {
            product = _field->mult(product, _field->pow(ja, fieldMax - jDegree));
          }
        }

        sum ^= _field->mult(product, getFunctionValue(f, a, fieldSize));
      }

      set(i, sum);

      /*
      if (zero(i))
      {
        set(BV32(0), f.get(BV32(0)));
        continue;
      }

      bv32 result = BV32(0);
      bv32 degmax = (bv32)bf::max32(_field->n());

      for (auto x : f.dom())
      {
        result ^= _field->mult(i < degmax ? _field->pow(x, degmax - i) : BV32(1), f.get(x));
      }

      set(i, result);
      */
    }   
  }

  std::ostream& operator<<(std::ostream& stream, const FPolynomial &f)
  {
    auto fieldSize = f.field()->n();
    auto fieldMax = bf::max32(fieldSize);
    bool isZero = true;
    
    for (auto i : f.dom())
    {
      auto c = f.get(i);

      if (zero(c))
      {
        continue;
      }
      
      if (!isZero)
      {
        stream << " + ";
      }

      isZero = false;

      if (c != BV32(1) || zero(i))
      {
        stream << c;
      }

      for (auto j = BV8(0); j < f.polynomialVars(); ++j)
      {
        auto jDegree = (i >> (j * fieldSize)) & fieldMax;

        if (!zero(jDegree))
        {
          stream << lexem::ParsedFormula::Variables[j];
        }

        if (jDegree > BV32(1))
        {
          stream << "**" << jDegree;
        }
      }
    }

    if (isZero)
    {
      stream << "0";
    }

    stream << " @ " << f.field()->Polynomial;

    return stream;
  }
}
