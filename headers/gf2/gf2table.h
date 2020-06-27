#pragma once

#include "bv.h"
#include "gf2/gf2.h"

#include <vector>

namespace bf
{
  class GF2Table : public GF2 
  {
  public:
    virtual bv32 mult(bv32 a, bv32 b) const noexcept;

    virtual bv32 pow(bv32 a, bv64 n) const noexcept;

    virtual bool tr(bv32 a) const noexcept;
  
    virtual bool isField();

    virtual bv32 generator() const;

  public:

  protected:
    explicit GF2Table(bv64 polynomial);

    friend class GF2Factory;
  private:
    bv8 _n;
    bv64 _nbelements;
    bv32 _nbdegs;
    bv32 _primary;
    bv32 _trcoeff;

    std::vector<bv32> _degrees;
    std::vector<bv32> _elements;
  };
}
