#include "func/bf.h"
#include "func/anf.h"
#include "iter/cnkiter.h"
#include "formula/parsedcalculator.h"
#include "linear/gjmatrix.h"

#include <sstream>

namespace
{
  struct AnfParsedCalculator : public lexem::ParsedCalculator<bf::BF>
  {
    AnfParsedCalculator(const ParsedFormula &f)
      :
      ParsedCalculator<bf::BF>(f)
    {
    }

    virtual void plus(bf::BF &a, const bf::BF& b)
    {
      a += b;
    }

    virtual void minus(bf::BF& a, const bf::BF& b)
    {
      plus(a, b);
    }

    virtual void minus(bf::BF& a)
    {
    }

    virtual void mult(bf::BF& a, const bf::BF& b)
    {
      a *= b;
    }

    virtual bf::BF convertConstant(bf::bv64 a)
    {
      bf::BF result(nVariables);

      result.set(bf::get(a, 0));

      return result;
    }

    virtual bf::BF convertVariable(unsigned int globalId)
    {
      bf::BF result(nVariables);

      for (auto x : result.dom())
      {
        if (bf::get(x, (bf::bv8)globalId))
        {
          result.set(x, true);
        }
      }

      return result;
    }
  };
}

namespace bf 
{
  Anf::Anf(const BF &bf)
    :
    BFBase(bf.MobiusTransform())
  {
  }

  int Anf::deg() const
  {
    int deg = -1;
    for (auto x : dom())
    {
      if (get(x))
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

  std::ostream& operator<<(std::ostream& stream, const Anf &anf)
  {
    return anf.write(stream, "x", 1, "", "", "", " + ");
  }

  std::ostream& Anf::write(std::ostream &stream, const char *variableSymbol, unsigned int startVar, const char *startIdx, const char *endIdx, const char *multiplySymbol, const char *sumSymbol) const
  {
    auto i = n();
    uint64_t nMonoms = 0;

    while (i > 0) 
    {
      for (auto cnk = CnkIter(n(), i); cnk; ++cnk)
      {
        if (get(cnk.first32())) 
        {
          if (nMonoms > 0)
            stream << sumSymbol;
          for (auto k = 0; k < i; ++k) 
          {
            stream << variableSymbol << startIdx << cnk[k] + startVar << endIdx;
          }
          nMonoms++;
        }
      }
      
      i--;
    }

    if (get(0))
    {
      stream << (nMonoms > 0 ? sumSymbol : "") << '1';
    }
    else if (nMonoms == 0)
    {
      stream << '0';
    }

    return stream;
  }

  bool Anf::setFormula(const std::string &formula)
  {
    std::istringstream sstream(formula);

    auto parsed = lexem::ParseFormula(sstream, {});

    if (!parsed.isOk() || !sstream.eof())
    {
      return false;
    }

    if (parsed.hasRationalContext || parsed.endWithSpecial || parsed.nVariables > (unsigned int)32)
    {
      return false;
    }

    AnfParsedCalculator calc(parsed);

    if (!calc.calculate())
    {
      return false;
    }

    *this = Anf(calc.value);

    return true;
  }

  Anf Anf::minAnnihilator(unsigned int upToDegree) const
  {
    std::vector<bv32> varsByNumber;
    std::vector<ChunkVector> system(size());

    bv32 nEquations = 0;
    std::vector<int64_t> monomialEquations(size(), -1);

    // generate supp of the anf
    std::vector<bv32> supp;

    for (auto x : dom())
    {
      if (get(x))
      {
        supp.push_back(x);
      }
    }

    // ordering monomials to get function of minimal degree as a first solution
    for (auto k = upToDegree; !zero(k); --k)
    {
      for (auto cnk = CnkIter(n(), k); cnk; ++cnk)
      {
        auto current = cnk.first32();

        varsByNumber.push_back(current);
      }
    }
    
    varsByNumber.push_back(BV32(0));

    // generating the system of equations
    for (size_t i = 0; i < supp.size(); ++i) 
    {
      for (size_t j = 0; j < varsByNumber.size(); ++j) 
      {
        auto monomial = supp[i] | varsByNumber[j];

        if (monomialEquations[monomial] < 0)
        {
          monomialEquations[monomial] = nEquations;
          system[nEquations].resize(varsByNumber.size());
          nEquations++;
        }

        auto equationNum = monomialEquations[monomial];

        system[equationNum].invert((bv32)j);
      }
    }
   
    // solving the system
    GJMatrix matrix;
    
    matrix.init(system.data(), nEquations, varsByNumber.size(), 1);
    
    if (matrix.nondegenerate())
    {
      return Anf();
    }

    auto solution = matrix.firstSolution();
 
    Anf result(n());

    for (size_t i = 0; i < varsByNumber.size(); ++i) 
    {
      result.set(varsByNumber[i], solution.get(i));
    }

    return result;
  }
}
