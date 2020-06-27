#include "vect/bvio.h"
#include "func/anf.h"
#include "func/vanf.h"
#include "func/fpolynomial.h"
#include "gf2/gf2factory.h"

#include <iostream>
#include <fstream>
#include <sstream>

unsigned int counter = 0;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << std::endl; return 1; } else { counter++; }

bool CheckPolynomial(std::string formula, std::string rmodel)
{
  bf::FPolynomial poly(bf::GF2Factory::createMappedDefault(BV8(4)));
  
  if (!poly.setFormula(formula))
  {
    std::cout << "Parsing failed: " << formula << std::endl;
    return false;
  }

  std::ostringstream stream;

  stream << poly;

  if (rmodel != stream.str())
  {
    std::cout << "Validatoin failed (actual, rmodel): " << std::endl << stream.str() << std::endl << rmodel << std::endl;
    return false;
  }

  return true;
}

bool CheckVAnf(std::string formula, std::string rmodel)
{
  bf::VAnf anf;

  if (!anf.setFormula(formula))
  {
    std::cout << "Parsing failed: " << formula << std::endl;
    return false;
  }

  std::ostringstream stream;

  stream << anf;

  if (rmodel != stream.str())
  {
    std::cout << "Validatoin failed (actual, rmodel): " << std::endl << stream.str() << std::endl << rmodel << std::endl;
    return false;
  }

  return true;
}

bool CheckAnf(std::string formula, std::string rmodel)
{
  bf::Anf anf;

  if (!anf.setFormula(formula))
  {
    std::cout << "Parsing failed: " << formula << std::endl;
    return false;
  }

  std::ostringstream stream;

  stream << anf;

  if (rmodel != stream.str())
  {
    std::cout << "Validatoin failed (actual, rmodel): " << std::endl << stream.str() << std::endl << rmodel << std::endl;
    return false;
  }

  return true;
}

int main()
{
  CHECK(CheckAnf(
    "0",
    "0"));

  CHECK(CheckAnf(
    "1",
    "1"));

  CHECK(CheckAnf(
    "x1 x10",
    "x1x10"));

  CHECK(CheckAnf(
    "x1 (x2 + x5)",
    "x1x2 + x1x5"));

  CHECK(CheckAnf(
    "x1 (x2 + x5) (x1+x5) + 1",
    "x1x2x5 + x1x2 + 1"));

  CHECK(CheckAnf(
    "(x1 + 1 + 1)(x6 + 1 + 1)(x2 + 1 + 1) + 1",
    "x1x2x6 + 1"));

  CHECK(CheckAnf(
    "(x1 + 1 + 1)(x12 + 1 + 1)(x2 + 1 + 1)(x14 + 1 + 1) + 1",
    "x1x2x12x14 + 1"));

  CHECK(CheckVAnf(
    "(x1 + 15)*(x2 + 15)(x3 + 15)(x4 + 15) + 0(15)", 
    "15x1x2x3x4 + 15x1x2x3 + 15x1x2x4 + 15x1x3x4 + 15x2x3x4 + 15x1x2 + 15x1x3 + 15x1x4 + 15x2x3 + 15x2x4 + 15x3x4 + 15x1 + 15x2 + 15x3 + 15x4 + 15"));
  
  CHECK(CheckPolynomial(
    "(10  + x**(-(1/2 + 1/3 + 7/(-2 + 4 * (9 + 2)))/2)) * (x^15 --- x**10) + @tr(8) @37",
    "1 + 10x**10 + 10x**15 + x**25 + x**30 @ 37"));

  CHECK(CheckPolynomial(
    "(10  + x**(-(1/2 + 1/3 + 7/(-2 + 4 * (9 + 2)))/2)) * (x^15 --- x**10) + @tr(8) @19",
    "1 + x**2 + x**7 + 10x**10 + 10x**15 @ 19"));

  CHECK(CheckPolynomial(
    "x**(-2**4 --10) + x**4 @19",
    "x**4 + x**9 @ 19"));

  CHECK(CheckPolynomial(
    "@tr(x**8 + x**2 + x**4) @19",
    "x + x**2 + x**4 + x**8 @ 19"));

  CHECK(CheckPolynomial(
    "@tr(x**5 + x**10) @19",
    "0 @ 19"));

  CHECK(CheckPolynomial(
    "9 @ 19",
    "9 @ 19"));
  
  CHECK(CheckPolynomial(
    "@ tr(-x + -x**2) + x (x ** 1 * x**-2) @ 19",
    "x**15 @ 19"));

  CHECK(CheckPolynomial(
    "xy @ 19",
    "xy @ 19"));

  CHECK(CheckPolynomial(
    "(1 + 3)x**2 y + (1 * 6)y*y xxz @ 7",
    "2x**2y + x**2y**2z @ 7"));

  CHECK(CheckPolynomial(
    "(x ** 2 + xxxy)(y + 3xxy**-1y**2) @ 37",
    "x**2y + 3x**4y + x**3y**2 + 3x**5y**2 @ 37"));

  CHECK(CheckPolynomial(
    "5 + (x + y)(y**30 x**31) @ 37",
    "5 + xy**30 + x**31y**31 @ 37"));

  CHECK(CheckPolynomial(
    "@gen x**1 + @  gen**-1 x**(0 + 2 * 1) @ 37",
    "2x + 18x**2 @ 37"));

  CHECK(CheckPolynomial(
    "@gen xy + @gen x * @gen (x + y ) + @gen@gen * @gen**2 / @gen @ 37",
    "8 + 4x**2 + 6xy @ 37"));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
