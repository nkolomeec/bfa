#pragma once

#include <cstdint>
#include <cmath>

namespace integer
{
  struct Rational
  {
    int64_t p;
    uint64_t q;

    Rational()
      :
      Rational(0)
    {
    }

    Rational(int64_t p)
    {
      this->p = p;
      q = 1;
    }

    uint64_t gcd(uint64_t a, uint64_t b) const
    {
      if (b == 0)
      {
        return a;
      }

      auto r = a % b;

      while (r > 0)
      {
        a = b;
        b = r;
        r = a % b;
      }

      return b;
    }

    uint64_t mod(int64_t elem, uint64_t modulo) const
    {
      return (uint64_t)(elem < 0 ? (modulo - (std::abs(elem) % modulo)) : elem % modulo);
    }

    uint64_t inversion(uint64_t elem, uint64_t modulo) const
    {
      auto a = modulo;
      auto b = elem;

      uint64_t a1 = 0;
      uint64_t b1 = 1;

      if (b == 0)
      {
        return 0;
      }

      auto r = a % b;
      uint64_t r1 = (a1 + (modulo - (a / b)) * b1) % modulo;

      while (r > 0)
      {
        auto q = a / b;
        
        a = b;
        b = r;
        r = a % b;

        a1 = b1;
        b1 = r1;
        r1 = (a1 + (modulo - q) * b1) % modulo;
      }

      return b1;
    }

    void simplify()
    {
      auto d = gcd((uint64_t)abs(p), q);

      p /= d;
      q /= d;
    }

    Rational& plus(Rational &b)
    {
      p = p * b.q + b.p * q;
      q *= b.q;

      simplify();

      return *this;
    }

    Rational& minus(Rational &b)
    {
      p = p * b.q - b.p * q;
      q *= b.q;

      simplify();

      return *this;
    }

    Rational& minus()
    {
      p = -p;

      return *this;
    }

    Rational& mult(Rational &b)
    {
      p *= b.p;
      q *= b.q;

      simplify();

      return *this;
    }

    bool isZero() const
    {
      return p == 0;
    }

    bool div(Rational &b)
    {
      if (isZero())
      {
        return false;
      }

      p *= b.q;
      q *= b.p;

      simplify();

      return true;
    }

    bool pow(Rational &b)
    {
      if (b.q != 1)
      {
        return false;
      }

      p = (int64_t)std::round(std::pow((double)p, (double)b.p));
      q = (uint64_t)std::round(std::pow((double)q, (double)b.p));

      return true;
    }
  };
}