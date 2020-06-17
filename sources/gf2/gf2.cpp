#pragma once

#include "gf2/gf2.h"
#include "gf2/bpolynomials.h"
#include "linear/linear.h"

#include <vector>

namespace bf
{
    bv32 GF2::mult(bv32 a, bv32 b) const noexcept
    {
      return (bv32)bpolynomials::mult(a, b, Polynomial);
    }

    bv32 GF2::pow(bv32 a, bv64 n) const noexcept
    {
      assert(a != 0 || n != 0);

      auto i = BV8(0);
      auto result = BV32(1);

      while (basis64(i) <= n)
      {
        if (bf::get(n, i))
        {
          result = mult(result, a);
        }

        a = mult(a, a);
        
        ++i;
      }

      return result;
    }

    bv32 GF2::inv(bv32 a) const noexcept
    {
      assert(a != 0);

      return pow(a, bf::basis64(this->n()) - 2);
    }

    bool GF2::tr(bv32 a) const noexcept
    {
      auto n = this->n();
      bv32 result = a;

      for (bv32 i = 1; i < n; ++i)
      {
        a = mult(a, a);
        result ^= a;
      }

      return result;
    }

    bv32 GF2::mod(bv64 a) const noexcept
    {
      return (bv32)bpolynomials::mod(a, Polynomial);
    }

    bool GF2::check() const
    {
      return bpolynomials::isIrreducible(Polynomial);
    }
}
