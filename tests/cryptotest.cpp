#include <iostream>

#include "func/anf.h"
#include "func/bf.h"
#include "func/crypto.h"

unsigned int counter = 0;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << std::endl; return 1; } else { counter++; }

bool CheckAI(const bf::BF &f, int immunity)
{
  auto annihilator = bf::crypto::ai(f);
  auto deg = annihilator.deg();

  bool success = true;
  bf::BF an(annihilator);

  for (auto x : f.dom())
  {
    if (f.get(x) && an.get(x))
    {
      success = false;
      break;
    }
  }

  if (!success)
  {
    success = true;
    for (auto x : f.dom())
    {
      if (!f.get(x) && an.get(x))
      {
        success = false;
        break;
      }
    }
  }

  if (!success)
  {
    std::cout << "Function annulator is not correct" << std::endl;
    return false;
  }

  if (deg != immunity)
  {
    std::cout << "AI is not correct, actual = " << (int)deg << std::endl;
    return false;
  }

  return true;
}

bool CheckDalaiFunction(int n, bf::bv32 midDivider, bool midValue)
{
  bf::BF result(n);

  for (auto x : result.dom())
  {
    auto wt = bf::weight(x);

    if ((wt << 1) > n)
    {
      result.set(x, true);
    }
    else if ((wt << 1) == n)
    {
      result.set(x, ((x % midDivider) != 0) ^ midValue);
    }
  }

  return CheckAI(result, (bf::bv8)((n >> 1) + (n & 1)));
}

bool CheckProperties(std::string formula, int deg, int ai, int ci, int resilency, int pc, bool balanced, bool sac, bf::bv64 gac, bf::bv32 nonlin)
{
  bf::Anf anf;
  if (!anf.setFormula(formula))
  {
    std::cout << "Formula is not parsed" << std::endl;
    return false;
  }

  bf::BF f(anf);

  auto actualDeg = bf::crypto::degree(f);
  if (actualDeg != deg)
  {
    std::cout << "Degree is not correct, actual = " << (int)actualDeg << std::endl;
    return false;
  }
  
  auto actualCI = bf::crypto::ci(f);
  if (actualCI != ci)
  {
    std::cout << "CI is not correct, actual = " << (int)actualCI << std::endl;
    return false;
  }

  auto actualres = bf::crypto::resilency(f);
  if (actualres != resilency)
  {
    std::cout << "Resilency is not correct, actual = " << (int)actualres << std::endl;
    return false;
  }

  auto actualPC = bf::crypto::pc(f);
  if (actualPC != pc)
  {
    std::cout << "PC is not correct, actual = " << (int)actualPC << std::endl;
    return false;
  }

  auto actualB = bf::crypto::balanced(f);
  if (actualB != balanced)
  {
    std::cout << "Balancedness is not correct, actual = " << (int)actualB << std::endl;
    return false;
  }

  auto actualSAC = bf::crypto::sac(f);
  if (actualSAC != sac)
  {
    std::cout << "SAC is not correct, actual = " << (int)actualSAC << std::endl;
    return false;
  }

  auto actualN = bf::crypto::nonlinearity(f);
  if (actualN != nonlin)
  {
    std::cout << "Nonlinearity is not correct, actual = " << (int)actualN << std::endl;
    return false;
  }

  auto actualGAC = bf::crypto::gac(f);
  if (actualGAC != gac)
  {
    std::cout << "GAC is not correct, actual = " << (int)actualGAC << std::endl;
    return false;
  }

  if (!CheckAI(f, ai))
  {
    return false;
  }

  return true;
}

int main()
{
  // check constants
  CHECK(CheckDalaiFunction(0, 1, false));
  CHECK(CheckDalaiFunction(0, 1, true));
  
  for (auto i = 1; i <= 11; i += 2)
  {
    CHECK(CheckDalaiFunction(i, 0, false));
  }

  for (auto i = 2; i <= 10; i += 2)
  {
    CHECK(CheckDalaiFunction(i, BV32(2), false));
    CHECK(CheckDalaiFunction(i, BV32(2), true));
    
    CHECK(CheckDalaiFunction(i, BV32(3), false));
    CHECK(CheckDalaiFunction(i, BV32(3), true));
  }

  CHECK(CheckProperties("0", -1, BV8(0),
    BV8(0), BV8(0), BV8(0), false, false, BV32(1),
    (bf::bv32)(0)));

  CHECK(CheckProperties("1", 0, BV8(0),
    BV8(0), BV8(0), BV8(0), false, false, BV32(1),
    (bf::bv32)(0)));

  CHECK(CheckProperties("1 + 0 x1x9", 0, BV8(0),
    BV8(9), BV8(0), BV8(0), false, false, BV32(1 << 9),
    (bf::bv32)(0)));

  CHECK(CheckProperties("x1 x2 + x3 x4", 2, BV8(2),
    BV8(0), BV8(0), BV8(4), false, true, BV32(0), 
    (bf::bv32)((1 << (4 - 1)) - (1 << (2 - 1)))));

  CHECK(CheckProperties("x1 x2 + x3 x4 + x5", 2, BV8(2), 
    BV8(0), BV8(0), BV8(0), true, false, (bf::bv32)(1 << 5),
    (bf::bv32)((1 << (5 - 1)) - (1 << (2)))));

  CHECK(CheckProperties("x1 + x2 + x3 + x4 + x5 + x6 + x7 + x8", 1, BV8(1),
    BV8(7), BV8(7), BV8(0), true, false, (bf::bv32)(1 << 8),
    (bf::bv32)(0)));

  CHECK(CheckProperties("(x1y1+x2y2+x3y3) + y1 y2 y3 + y1 y2 + y1 y3 + 1", 3, BV8(2),
    BV8(0), BV8(0), BV8(6), false, true, BV32(0),
    (bf::bv32)((1 << (6 - 1)) - (1 << (3 - 1)))));

  CHECK(CheckProperties("(x1(y1+y2y3y4)+x2(y2+y3y4)+x3y3+x4y4) + y1y2y3 + y1y2y4 + y1y2y4 + y2y3y4", 4, BV8(3),
    BV8(0), BV8(0), BV8(8), false, true, BV32(0),
    (bf::bv32)((1 << (8 - 1)) - (1 << (4 - 1)))));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
