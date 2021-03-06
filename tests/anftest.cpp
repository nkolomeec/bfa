#include "func/bf.h"
#include "func/anf.h"
#include "vect/bvio.h"

#include <iostream>
#include <vector>
#include <sstream>

unsigned int counter;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << ": " << #x << " failed." << std::endl; return 1; } else { counter++; }

bool CheckAnf(const std::vector<bf::bv32> monomials)
{
  bf::Anf resizableAnf;

  std::stringstream ss;
  bool empty = true;

  if (monomials.empty())
  {
    ss << "0";
  }

  for (auto monomial : monomials)
  {
    if (resizableAnf.dom().max() < monomial)
    {
      resizableAnf.resize(bf::size(monomial));
    }

    resizableAnf.set(monomial, resizableAnf.get(monomial) ^ true);

    if (!empty)
    {
      ss << " + ";
    }

    if (bf::zero(monomial))
    {
      ss << "1";
    }
    else
    {
      for (bf::bv8 i = 0; i < BV8(32); ++i)
      {
        if (bf::get(monomial, i))
        {
          ss << "x" << (int)(i + 1);
        }
      }
    }

    empty = false;
  }

  if (resizableAnf.n() > 0)
  {
    ss << " + 0 x1 x" << (int)resizableAnf.n();
  }

  bf::Anf anf(resizableAnf.n());

  for (auto monomial : monomials)
  {
    anf.set(monomial, anf.get(monomial) ^ true);
  }
  
  bf::Anf formulaAnf;
  if (!formulaAnf.setFormula(ss.str()))
  {
    std::cout << "Parsing formula is failed" << std::endl;
    return false;
  }

  bf::BF f(formulaAnf);
  bf::Anf fanf(f);

  for (auto x : resizableAnf.dom())
  {
    if (anf.get(x) != resizableAnf.get(x))
    {
      return false;
    }

    if (fanf.get(x) != anf.get(x))
    {
      return false;
    }
  }

  return true;
}

int main()
{
  CHECK(CheckAnf({ 0 }));

  CHECK(CheckAnf({ 4 }));

  CHECK(CheckAnf({ 7 }));

  CHECK(CheckAnf({0, 1, 2, 3, 4, 5, 6, 7}));

  CHECK(CheckAnf({ 0, 1, 2, 3, 4, 5, 6, 7 }));

  CHECK(CheckAnf({ 0, 1, 2, 3, 4, 5, 6, 7, 63}));

  CHECK(CheckAnf({ 0, 1, 2, 3, 4, 5, 6, 7, 64 }));

  CHECK(CheckAnf({ 0, 1, 2, 3, 4, 5, 6, 7, 128 }));

  CHECK(CheckAnf({ 10, 100, 200, 3, 4, 5, 6, 127, 255 }));

  CHECK(CheckAnf({ 10, 100, 200, 3, 4, 5, 6, 127, 255, 2047 }));

  CHECK(CheckAnf({ 5, 101, 224, 3, 4, 5, 6, 127, 255, 9000 }));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
