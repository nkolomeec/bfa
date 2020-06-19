#pragma once

#include "bv.h"
#include <memory>

namespace bf
{
  class GF2 
  {
  public:
    inline int n() const noexcept
    {
      return bf::lead(Polynomial);
    }

    inline bv32 sum(bv32 a, bv32 b) const noexcept
    {
      return bf::sum(a, b);
    }

    virtual bv32 mult(bv32 a, bv32 b) const noexcept;

    virtual bv32 inv(bv32 a) const noexcept;

    virtual bv32 pow(bv32 a, bv64 n) const noexcept;

    virtual bool tr(bv32 a) const noexcept;
  
    virtual bool check() const;

    bv32 mod(bv64 a) const noexcept;

  public:
    constexpr explicit GF2(bv64 polynomial)
      : Polynomial(polynomial)
    {
    }

  public:
    const bv64 Polynomial;

    friend class GF2Factory;
  };
}
