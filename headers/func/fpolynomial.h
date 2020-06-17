#pragma once

#include "vect/chunkvector.h"
#include "vbfbase.h"
#include "gf2/gf2.h"
#include "func/vbf.h"

#include <vector>
#include <memory>
#include <string>

namespace bf
{
  class FPolynomial : public VBFBase
  {
    std::shared_ptr<const GF2> _field;
    bv8 _nPolynomialVars;

  public:
    inline FPolynomial() = delete;

    inline explicit FPolynomial(const std::shared_ptr<const GF2> &gf2, bv8 nPolynomialVars = 0)
      :
      VBFBase(nPolynomialVars * gf2->n(), gf2->n()),
      _nPolynomialVars(nPolynomialVars),
      _field(gf2)
    {
    }

    FPolynomial(std::shared_ptr<const GF2> gf2, bv8 nPolynomialVars, const VBF &f);

    bool setFormula(std::string formula);

    std::shared_ptr<const GF2> field() const
    {
      return _field;
    }

    bv8 polynomialVars() const
    {
      return _nPolynomialVars;
    }
  };

  std::ostream& operator<<(std::ostream& stream, const FPolynomial &poly);

}