#include <iostream>

#include "gf2/gf2.h"
#include "gf2/bpolynomials.h"
#include "gf2/gf2factory.h"

unsigned int counter;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << ": " << #x << " failed." << std::endl; return 1; } else { counter++; }

bool CheckDefaultPolynomials()
{
  bf::bv64 polynomials[32] = {
    3, 7, 11, 19, 37, 67, 131, 283,
    515, 1033, 2053, 4105, 8219, 16417, 32771, 65579,
    131081, 262153, 524327, 1048585, 2097157, 4194307, 8388641, 16777243,
    33554441, 67108891, 134217767, 268435459, 536870917, 1073741827, BV64(2147483657), BV64(4294967437)
  };

  for (auto poly : polynomials)
  {
    bf::GF2 gf2(poly);

    if (!gf2.check())
    {
      return false;
    }
  }

  return true;
}

bool CheckMappedDefault()
{
  bf::bv64 polynomials[] = {
    3, 7, 11, 19, 37, 67, 131, 283,
    515, 1033, 2053, 4105, 8219, 16417, 32771, 65579,
    131081, 262153 //, 524327, 1048585, 2097157, 4194307, 8388641, 16777243
  };

  for (auto poly : polynomials)
  {
    auto field = bf::GF2Factory::createMapped(poly, false);

    if (field == nullptr)
    {
      return false;
    }
  }

  return true;
}

int main()
{
  CHECK(CheckDefaultPolynomials());
  CHECK(CheckMappedDefault());

  std::cout << counter << " successful" << std::endl;

  return 0;
}
