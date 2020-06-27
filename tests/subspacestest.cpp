#include "gf2/gf2factory.h"
#include "func/bf.h"
#include "func/anf.h"
#include "func/fpolynomial.h"
#include "props/subspacebfiter.h"
#include "vect/linearcombinations.h"

#include <iostream>
#include <chrono>

unsigned int counter;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << ": " << #x << " failed." << std::endl; return 1; } else { counter++; }

bool CheckGJB(const bf::bv16 * basis, bf::bv16 count, bool affine)
{
  if (affine && bf::zero(count))
  {
    return false;
  }

  bf::bv16 mask = BV16(0);

  int gjcount = affine ? count - 1 : count;

  for (auto i = 0; i < gjcount; ++i)
  {
    if (bf::zero(basis[i]))
    {
      return false;
    }

    if ((basis[i] & mask) != 0)
    {
      return false;
    }

    if (i > 0 && basis[i - 1] >= basis[i])
    {
      return false;
    }

    mask ^= bf::basis16(bf::lead(basis[i]));
  }

  if (affine && ((basis[count - 1] & mask) != 0))
  {
    return false;
  }

  return true;
}

bool CheckConstant(const bf::BF &f, bf::bv16 coset, const bf::bv16* basis, bf::bv16 count, bool value)
{
  for (auto& combs : bf::LinearCombinations<bf::bv16>(coset, basis, count))
  {
    if (f.get(combs.combination()) != value)
    {
      return false;
    }
  }
  
  return true;
}

bool CheckAffine(const bf::BF& f, bf::bv16 coset, const bf::bv16* basis, bf::bv16 count, bool value)
{
  if (count == 0)
  {
    return false;
  }

  if (!CheckConstant(f, coset, basis, count - BV16(1), value))
  {
    return false;
  }

  if (!CheckConstant(f, coset ^ basis[count - BV16(1)], basis, count - BV16(1), value ^ true))
  {
    return false;
  }

  return true;
}

bool CheckOutput(const bf::BF &f, const bf::SubspaceBFIter& iter)
{
  if (!CheckGJB(iter.basis(), iter.dim(), iter.affine()))
  {
    std::cout << "Basis is not in GJ form" << std::endl;
    return false;
  }

  for (auto i = 0; i < iter.nzeros(); ++i)
  {
    if (!iter.affine() && i > 0 && iter.zeros()[i] <= iter.zeros()[i - 1])
    {
      std::cout << "Cosets are not ordered" << std::endl;
      return false;
    }

    auto check = iter.affine() 
      ? CheckAffine(f, iter.zeros()[i], iter.basis(), iter.dim(), false) 
      : CheckConstant(f, iter.zeros()[i], iter.basis(), iter.dim(), false);

    if (!check)
    {
      std::cout << "Coset " << i << " is wrong" << std::endl;
      return false;
    }
  }

  for (auto i = 0; i < iter.nones(); ++i)
  {
    if (!iter.affine() && i > 0 && iter.ones()[i] <= iter.ones()[i - 1])
    {
      std::cout << "Cosets are not ordered" << std::endl;
      return false;
    }

    auto check = iter.affine()
      ? CheckAffine(f, iter.ones()[i], iter.basis(), iter.dim(), true)
      : CheckConstant(f, iter.ones()[i], iter.basis(), iter.dim(), true);

    if (!check)
    {
      std::cout << "Coset " << i << " is wrong" << std::endl;
      return false;
    }
  }

  return true;
}

uint64_t subspacesCount(int n, int startDim, int stopDim, bool cosets)
{
  uint64_t subspacesForDim = 1;
  uint64_t result = startDim <= 0 ? (cosets ? UINT64_C(1) << n : 1) : 0;

  for (auto i = 0; i < std::min(stopDim, n); ++i)
  {
    subspacesForDim = subspacesForDim * ((UINT64_C(1) << (n - i)) - 1) / ((UINT64_C(1) << (i + 1)) - 1);

    if (i + 1 >= startDim)
    {
      result += cosets ? (UINT64_C(1) << (n - i - 1)) * subspacesForDim : subspacesForDim;
    }
  }

  return result;
}

uint64_t maxCountForBent(int n, int startDim, int stopDim, bool cosets, bool affine, bool byMax = false)
{
  int k = n / 2;
  uint64_t subspacesForDim = 1;
  uint64_t result = startDim <= 0 ? (cosets ? UINT64_C(1) << n : 1) : 0;

  if (affine || cosets)
  {
    for (auto i = 0; i < std::min(stopDim, k); ++i)
    {
      subspacesForDim = subspacesForDim * ((UINT64_C(1) << (2 * (k - i))) - 1) / ((UINT64_C(1) << (i + 1)) - 1);

      if (i + 1 >= startDim)
      {
        if (affine && cosets)
        {
          result += (UINT64_C(1) << (n - i - 1)) * subspacesForDim ;
        }
        else if (cosets)
        {
          result += (UINT64_C(1) << (n - 2 *(i + 1))) * subspacesForDim;
        }
        else
        {
          result += subspacesForDim;
        }
      }
    }
  }
  else
  {
    auto forZero = subspacesForDim;
    auto forOne = subspacesForDim;
    for (auto i = 0; i < std::min(stopDim, k); ++i)
    {
      auto diff = UINT64_C(1) << (k - i - 1);
      
      forZero = forZero * ((UINT64_C(1) << (2 * (k - i) - 1)) + diff - 1) / ((UINT64_C(1) << (i + 1)) - 1);
      forOne = forOne * ((UINT64_C(1) << (2 * (k - i) - 1)) - diff - 1) / ((UINT64_C(1) << (i + 1)) - 1);

      if (i + 1 >= startDim)
      {
        result += byMax ? forZero : forOne;
      }
    }
  }

  return result;
}

bool CheckDimensionVariety(const bf::BF f, bool cosets, bool affine)
{
  std::vector<int64_t> countByDim((size_t)f.n() + 1);

  bf::SubspaceBFIter iter(f, cosets, affine, 0, f.n());

  for (iter.begin(); iter; ++iter)
  {
    if (!CheckOutput(f, iter))
    {
      return false;
    }
    
    countByDim[iter.dim()] += !cosets ? 1 : iter.nzeros() + iter.nones();
  }

  for (auto i = 0; i <= f.n(); ++i)
  {
    int64_t count = 0;

    bf::SubspaceBFIter iter(f, cosets, affine, i, i);

    for (iter.begin(); iter; ++iter)
    {
      if (iter.dim() != i)
      {
        return false;
      }

      count += !cosets ? 1 : iter.nzeros() + iter.nones();
    }

    if (count != countByDim[i])
    {
      return false;
    }
  }

  if (!affine)
  {
    for (auto i = 0; i < 2; ++i)
    {
      bf::SubspaceBFIter iter(f, cosets, 0, f.n(), (bool)i);

      for (iter.begin(); iter; ++iter)
      {
        countByDim[iter.dim()] -= !cosets ? 1 : iter.nzeros() + iter.nones();
      }
    }

    for (auto x : countByDim)
    {
      if (x != 0)
      {
        return false;
      }
    }
  }

  return true;
}

bool CheckDimensionVariety(std::string formula)
{
  bf::Anf anf;

  if (!anf.setFormula(formula))
  {
    return false;
  }

  bf::BF f(anf);

  for (auto i = 0; i < 2; ++i)
  {
    for (auto j = 0; j < 2; ++j)
    {
      if (!CheckDimensionVariety(f, (bool)i, (bool)j))
      {
        return false;
      }
    }
  }

  return true;
}

std::vector<uint64_t> subspaceCount(const bf::BF& f, bool cosets, bool affine, int startDim, int stopDim, bool onlyZeros = false)
{
  std::vector<uint64_t> countByDim((size_t)f.n() + 1);

  if (!onlyZeros)
  {
    bf::SubspaceBFIter iter(f, cosets, affine, startDim, stopDim);

    for (iter.begin(); iter; ++iter)
    {
      countByDim[iter.dim()] += !cosets ? 1 : iter.nzeros() + iter.nones();
    }
  }
  else
  {
    bf::SubspaceBFIter iter(f, cosets, startDim, stopDim, false);

    for (iter.begin(); iter; ++iter)
    {
      countByDim[iter.dim()] += !cosets ? 1 : iter.nzeros() + iter.nones();
    }
  }

  return countByDim;
}

bool CheckAffine(std::string formula, bool isConstant)
{
  bf::Anf anf;

  if (!anf.setFormula(formula))
  {
    return false;
  }

  bf::BF f(anf);

  for (auto i = 0; i < 2; ++i)
  {
    for (auto j = 0; j < 2; ++j)
    {
      auto count = subspaceCount(f, (bool)i, (bool)j, 0, f.n());

      for (auto k = 0; k < count.size(); ++k)
      {
        auto expected = !isConstant && j == 0 ?
          (i == 0 
            ? subspacesCount(f.n() - 1, k, k, false) : 2 * subspacesCount(f.n() - 1, k, k, true))
          : subspacesCount(f.n(), k, k, (bool)i);

        if (expected != count[k])
        {
          return false;
        }
      }
    }
  }

  return true;
}

bool CheckQuadraticBent(std::string formula)
{
  bf::Anf anf;

  if (!anf.setFormula(formula))
  {
    return false;
  }

  bf::BF f(anf);

  auto wt = f.weight();
  
  bool byMax = f.get(0) ^ (2 * wt < f.size());

  for (auto i = 0; i < 2; ++i)
  {
    for (auto j = 0; j < 2; ++j)
    {
      auto count = subspaceCount(f, (bool)i, (bool)j, 0, f.n());

      for (auto k = 0; k < count.size(); ++k)
      {
        auto expected = maxCountForBent(f.n(), k, k, (bool)i, (bool)j, byMax);

        if (expected != count[k])
        {
          return false;
        }
      }
    }
  }

  return true;
}

bool CheckPolynomialBent(std::string formula, int n, bool cosets, bool affine, bool onlyZeros, const std::vector<int> &expected)
{
  bf::VBF F;
  if (!bf::FPolynomial::setFormula(formula, bf::GF2Factory::createMappedDefault(n), F))
  {
    return false;
  }

  bf::BF f(F.coordinate(0));

  auto count = subspaceCount(f, cosets, affine, 0, f.n(), onlyZeros);

  for (auto k = 0; k < count.size(); ++k)
  {
    if (expected[k] != count[k])
    {
      return false;
    }
  }

  return true;
}

int main()
{
  CHECK(CheckDimensionVariety("0"));
  CHECK(CheckDimensionVariety("1"));
  CHECK(CheckDimensionVariety("0 + 0 x1x5"));
  CHECK(CheckDimensionVariety("1 + 0 x1x5"));
  CHECK(CheckDimensionVariety("x1x2x3 + x1x4"));
  CHECK(CheckDimensionVariety("x1x2 + x3x4 + 1"));
  CHECK(CheckDimensionVariety("x1x2 + x3x4 + x5x6 + x5"));
  CHECK(CheckDimensionVariety("x1x2 + x3x4 + x5x6 + x7"));


  CHECK(CheckAffine("0 + 0 x1 x4", true));
  CHECK(CheckAffine("1 + 0 x1 x5", true));
  CHECK(CheckAffine("x1 + 0 x2", false));
  CHECK(CheckAffine("x1 + x3", false));
  CHECK(CheckAffine("x1 + x5", false));
  CHECK(CheckAffine("0 x1 + x5", false));
  CHECK(CheckAffine("x1 + x3 + x6", false));

  CHECK(CheckQuadraticBent("x1 x2"));
  CHECK(CheckQuadraticBent("x1 x2 + x3x4 + x1 + x2"));
  CHECK(CheckQuadraticBent("x1 x2 + x3x4 + x5x6 + 1"));
  CHECK(CheckQuadraticBent("x1 x2 + x3x4 + x5x6 + x7 x8 + x1 + x3 + x5 + x7 + 1"));

  CHECK(CheckPolynomialBent("@tr(2 x**57) @ 283",
    8, true, true, false, 
    { 256, 32640, 342720, 172944, 493, 0, 0, 0, 0 }));

  CHECK(CheckPolynomialBent("@tr(2 x**57) @ 283",
    8, true, false, false,
    { 256, 16320, 85680, 21618, 55, 0, 0, 0, 0 }));

  CHECK(CheckPolynomialBent("@tr(2 x**57) @ 283",
    8, true, false, true,
    { 136, 9180, 53550, 16188, 55, 0, 0, 0, 0 }));

  CHECK(CheckPolynomialBent("@tr(2 x**57) + 1 @ 283",
    8, true, false, true,
    { 120, 7140, 32130, 5430, 0, 0, 0, 0, 0 }));

  CHECK(CheckPolynomialBent("@tr(2 x**57) @ 283",
    8, false, true, false,
    { 1, 255, 5355, 5379, 55, 0, 0, 0, 0}));

  CHECK(CheckPolynomialBent("@tr(2 x**57) @ 283",
    8, false, false, false,
    { 1, 135, 1575, 957, 28, 0, 0, 0, 0 }));

  CHECK(CheckPolynomialBent("@tr(@gen x**57) @ 1033",
    10, true, true, false,
    { 1024, 523776, 22260480, 49395840, 1257368, 9, 0, 0, 0, 0, 0 }));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
