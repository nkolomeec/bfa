#include <iostream>

#include "func/anf.h"
#include "func/bf.h"
#include "vect/bvio.h"

unsigned int counter = 0;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << std::endl; return 1; } else { counter++; }

bool CheckWalsh(const bf::BF &f)
{
  auto walsh = f.Walsh();

  if (f.n() != walsh.n())
  {
    return false;
  }

  for (auto x : walsh.dom())
  {
    if (f.Walsh(x) != walsh[x])
    {
      return false;
    }
  }

  auto autocorrelation = walsh.autocorrelation();

  for (auto x : f.dom())
  {
    if (f.autocorrelation(x) != autocorrelation[x])
    {
      return false;
    }
  }

  return true;
}

bool CheckWalsh(std::string formula)
{
  bf::Anf anf;

  if (!anf.setFormula(formula))
  {
    return false;
  }

  bf::BF f(anf);

  return CheckWalsh(f);
}

int main()
{
  bf::BF f;

  CHECK(CheckWalsh(f));

  f.set(BV32(0), true);
  CHECK(CheckWalsh(f));

  f.resize(5);
  CHECK(CheckWalsh(f));

  f.resize(6);
  CHECK(CheckWalsh(f));

  f.resize(10);
  CHECK(CheckWalsh(f));

  f.resize(4);
  f.set(0, false);
  CHECK(CheckWalsh(f));

  CHECK(CheckWalsh("x1 + x2 + x3"));

  CHECK(CheckWalsh("x1 + x2 + x10"));

  CHECK(CheckWalsh("x1x2x3 + x1 + x3"));

  CHECK(CheckWalsh("x1x2x3 + x1 + x3 + x10"));

  CHECK(CheckWalsh("x1x2 + x3x4 + x5x6"));

  CHECK(CheckWalsh("x1x2 + x3x4 + x5x6 + x7x8 + x9x10"));

  CHECK(CheckWalsh("x1x2 + x3x4 + x5x6 + x7"));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
