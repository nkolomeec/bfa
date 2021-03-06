#include <iostream>

#include "func/vanf.h"
#include "func/vbf.h"
#include "func/fpolynomial.h"
#include "func/crypto.h"
#include "gf2/gf2factory.h"
#include "props/ddt.h"

unsigned int counter = 0;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << std::endl; return 1; } else { counter++; }

bool CheckProperties(const bf::VBF & f, int32_t deg, bf::bv8 ai, bf::bv32 nonlin, bf::bv32 uniformity)
{
  auto actualDeg = bf::crypto::degree(f);
  if (actualDeg != deg)
  {
    std::cout << "Degree is not correct, actual = " << (int)actualDeg << std::endl;
    return false;
  }

  auto actualAI = bf::crypto::componentAi(f);
  if (actualAI != ai)
  {
    std::cout << "AI is not correct, actual = " << (int)actualAI << std::endl;
    return false;
  }

  auto actualN = bf::crypto::nonlinearity(f);
  if (actualN != nonlin)
  {
    std::cout << "Nonlinearity is not correct, actual = " << (int)actualN << std::endl;
    return false;
  }

  auto actualUniformity = bf::crypto::uniformity(f);
  if (actualUniformity != uniformity)
  {
    std::cout << "Uniformity is not correct, actual = " << (int)actualUniformity << std::endl;
    return false;
  }

  if ((uniformity <= 2) != bf::crypto::uniformity(f, BV32(2)))
  {
    std::cout << "Checking APN is not correct" << std::endl;
  }

  if ((uniformity <= 4) != bf::crypto::uniformity(f, BV32(4)))
  {
    std::cout << "Checking 4-uniformity is not correct" << std::endl;
  }

  return true;
}

bool CheckPolynomProperties(std::string formula, bf::bv8 n, int32_t deg, bf::bv8 ai, bf::bv32 nonlin, bf::bv32 uniformity)
{
  bf::FPolynomial poly(bf::GF2Factory::createMappedDefault(n));

  if (!poly.setFormula(formula))
  {
    std::cout << "Formula is not parsed" << std::endl;
    return false;
  }

  bf::VBF f(poly);

  return CheckProperties(f, deg, ai, nonlin, uniformity);
}

bool CheckDDT(std::string formula, bf::bv8 n, bf::bv32 count)
{
  bf::FPolynomial poly(bf::GF2Factory::createMappedDefault(n));

  if (!poly.setFormula(formula))
  {
    std::cout << "Formula is not parsed" << std::endl;
    return false;
  }

  bf::VBF f(poly);

  bf::DDT ddt(f);

  auto result = ddt.restore();

  for (auto& func : result)
  {
    bf::DDT ddtcheck(func);

    if (ddtcheck != ddt)
    {
      std::cout << "Result function is not satisfied DDT" << std::endl;
      return false;
    }
  }

  if (result.size() != count)
  {
    std::cout << "Number of DDT classes is not correct, actual = " << result.size() << std::endl;
    return false;
  }

  return true;
}

int main()
{
  CHECK(CheckPolynomProperties("x**-1", BV8(1), 1, BV8(1),
    BV32(0),
    BV32(2)));

  CHECK(CheckPolynomProperties("x**-1", BV8(3), 2, BV8(1),
    (bf::bv32)((1 << (3 - 1)) - (1 << ((3 - 1) >> 1))),
    BV32(2)));

  CHECK(CheckPolynomProperties("x**-1", BV8(5), 4, BV8(1),
    BV32(10),
    BV32(2)));

  CHECK(CheckPolynomProperties("x**-1", BV8(7), 6, BV8(1),
    BV32(54),
    BV32(2)));

  CHECK(CheckPolynomProperties("x**-1", BV8(9), 8, BV8(1),
    BV32(234),
    BV32(2)));

  CHECK(CheckPolynomProperties("x**-1", BV8(2), 1, BV8(1),
    BV32(0),
    BV32(4)));

  CHECK(CheckPolynomProperties("x**-1", BV8(4), 3, BV8(1),
    (bf::bv32)((1 << (4 - 1)) - (1 << (2))),
    BV32(4)));

  CHECK(CheckPolynomProperties("x**-1", BV8(6), 5, BV8(1),
    (bf::bv32)((1 << (6 - 1)) - (1 << (3))),
    BV32(4)));

  CHECK(CheckPolynomProperties("x**-1", BV8(8), 7, BV8(1),
    (bf::bv32)((1 << (8 - 1)) - (1 << (4))),
    BV32(4)));

  CHECK(CheckDDT("x**-1", BV8(5), BV32(1)));

  CHECK(CheckDDT("x**3", BV8(4), BV32(4)));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
