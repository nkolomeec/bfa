#pragma once

#include "bv.h"
#include "linear/linear.h"

namespace bpolynomials
{
  // multiplication of a by x modulo mod
  // deg(a) should be less than deg(mod)
  inline bf::bv64 shift(bf::bv64 a, bf::bv64 mod)
  {
    a = a << 1;

    if (bf::sameLeads(a, mod))
    {
      a ^= mod;
    }

    return a;
  }

  inline bf::bv64 mult(bf::bv64 a, bf::bv64 b, bf::bv64 mod)
  {
    auto poly = std::max(a, b);
    auto pi = std::min(a, b);

    auto result = BV64(0);

    while (pi > 0)
    {
      if (bf::get(pi, 0))
      {
        result ^= poly;
      }

      poly = shift(poly, mod);

      pi = pi >> 1;
    }

    return result;
  }

  inline bf::bv64 mod(bf::bv64 a, bf::bv64 mod, bf::bv64& odiv)
  {
    if (mod <= 1)
    {
      odiv = mod;
      return 0;
    }

    auto xn = BV64(1);
    auto xnmod = mod;

    while (xnmod < a)
    {
      xn <<= 1;
      xnmod <<= 1;
    }

    if (!bf::sameLeads(a, xnmod))
    {
      if (xnmod == mod)
      {
        odiv = 0;
        return a;
      }

      xn >>= 1;
      xnmod >>= 1;
    }

    odiv = 0;
    auto result = a;

    while (xnmod >= mod)
    {
      if (bf::sameLeads(result, xnmod))
      {
        odiv ^= xn;
        result ^= xnmod;
      }

      xn >>= 1;
      xnmod >>= 1;
    }

    return result;
  }

  inline bf::bv64 mod(bf::bv64 a, bf::bv64 modulo)
  {
    bf::bv64 div;

    return mod(a, modulo, div);
  }

  inline bf::bv64 derivative(bf::bv64 a)
  {
    return (a & BV64(0xAAAAAAAAAAAAAAAA)) >> 1;
  }

  inline bf::bv64 gcd(bf::bv64 a, bf::bv64 b)
  {
    if (b == 0)
    {
      return a;
    }

    auto r = mod(a, b);

    while (r > 0)
    {
      a = b;
      b = r;
      r = mod(a, b);
    }

    return b;
  }

  // part of Berlecamp algorithm
  inline bool isIrreducible(bf::bv64 a)
  {
    if (a == 0)
    {
      return false;
    }

    auto deriv = derivative(a);

    if (deriv == 0)
    {
      return false;
    }

    auto divisor = gcd(a, deriv);

    if (divisor != 1)
    {
      return false;
    }

    auto n = bf::lead(a);

    std::vector<bf::bv64> matrix(n);

    auto xiq = BV64(1);

    for (bf::bv8 i = 0; i < n; ++i)
    {
      matrix[i] = xiq ^ bf::basis64(i);
      xiq = shift(shift(xiq, a), a);
    }

    auto rank = bf::rank<bf::bv64>(matrix.data(), n);

    return rank >= n - 1;
  }
}
