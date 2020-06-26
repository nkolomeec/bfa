#include "bv.h"
#include "vect/domain.h"
#include "vect/domainmask.h"
#include "vect/bvio.h"
#include "vect/linearcombinations.h"

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>

unsigned int counter;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << ": " << #x << " failed." << std::endl; return 1; } else { counter++; }

template <typename T>
bool CheckDom(const bf::Domain<T> &iter, int n)
{
  bf::bv64 i = 0;

  for (auto x : iter)
  {
    if ((bf::bv64)x != i)
    {
      return false;
    }
    ++i;
  }

  if (i != bf::basis64(n))
  {
    return false;
  }

  return true;
}

bool CheckDom8()
{
  for (auto n = 0; n <= 8; ++n)
  {
    if (!CheckDom(bf::Domain<bf::bv8>(n), n))
    {
      return false;
    }

    if (!CheckDom(bf::Domain<bf::bv32>(n), n))
    {
      return false;
    }

    if (!CheckDom(bf::Domain<bf::bv64>(n), n))
    {
      return false;
    }
  }

  return true;
}

bool CheckMask(bf::bv32 mask)
{
  bf::bv64 i = 0;
  auto umask = ~mask;
  bf::bv64 previous = 0;

  for (auto x : bf::DomainMask<bf::bv32>(mask))
  {
    if (!bf::zero(x & umask))
    {
      return false;
    }

    if (i > 0 && previous >= x)
    {
      return false;
    }

    previous = x;

    ++i;
  }

  if (i != bf::basis64(bf::weight(mask)))
  {
    return false;
  }

  return true;
}

template <typename T>
bool CheckIO(const T& x, int length = -1)
{
  std::stringstream stream;
  stream << "0x" << std::hex << (bf::bv64)x << " " << std::dec << (bf::bv64)x << std::endl << "0" << std::oct << (bf::bv64)x << "       0b" << bf::bv(x, length);
  
  stream.seekg(0, stream.beg);

  for (auto i = 0; i < 4; ++i)
  {
    T y;
    if (!(stream >> bf::bv(y)) || y != x)
    {
      std::cout << "x = " << (bf::bv64)x << std::endl;
      return false;
    }
  }

  return true;
}

bool CheckWrongIO()
{
  std::stringstream stream;
  stream << "0xFFFFFFFFFFFFFFFF    adfa";
  
  stream.seekg(0, stream.beg);
  bf::bv8 v8;
  if (stream >> bf::bv(v8))
  {
    return false;
  }

  stream.clear();
  stream.seekg(0, stream.beg);
  bf::bv32 v32;
  if (stream >> bf::bv(v32))
  {
    return false;
  }

  stream.clear();
  stream.seekg(0, stream.beg);
  bf::bv64 v64;
  if (!(stream >> bf::bv(v64)))
  {
    return false;
  }

  stream.clear();
  stream.seekg(0, stream.beg);
  if (stream >> bf::bv(v64, BV8(63)))
  {
    return false;
  }

  return true;
}

template <typename T>
bool CheckZero(const T& x)
{
  return (x == (T)0 && bf::zero(x)) || (x != (T)0 && !bf::zero(x));
}

template <typename T>
bool CheckWeight(const T& x)
{
  auto wt = bf::weight(x);

  bf::bv8 w = 0;

  auto y = x;

  while (!bf::zero(y))
  {
    y = y & (y - 1);
    ++w;
  }

  return w == wt;
}

template <typename T>
bool CheckSize(const T& x)
{
  auto sz = bf::size(x);

  bf::bv8 w = 0;

  auto y = x;

  while (!bf::zero(y))
  {
    y >>= 1;
    ++w;
  }

  return w == sz;
}

bool CheckDeBruijn()
{
  for (auto i = 0; i < 64; ++i)
  {
    auto j = bf::trailingZeros(bf::basis64(i));

    if (i != j)
    {
      return false;
    }
  }

  return bf::trailingZeros(0) == 0;
}

bool CheckLinearCombinations(int n, bool skipZero)
{
  std::vector<bf::bv64> basis(n);
  std::vector<bf::bv64> check(bf::basis64(n));

  for (auto i = 0; i < n; ++i)
  {
    basis[i] = bf::max64(n - i);
  }

  bf::bv64 start = BV64(0xABCDEF);

  for (auto& comb : bf::LinearCombinations<bf::bv64>(start, basis.data(), n, skipZero))
  {
    auto &a = comb.combination();
    check[a^start]++;

    auto coords = comb.coordinates();

    auto c = start;
    for (auto i = 0; i < 64; ++i)
    {
      if (bf::get(coords, i))
      {
        c ^= basis[i];
      }
    }

    if (a != c)
    {
      return false;
    }
  }

  for (int i = skipZero; i < check.size(); ++i)
  {
    if (check[i] != 1)
    {
      return false;
    }
  }

  return true;
}

int main()
{
  CHECK(CheckDom8());

  for (auto x : bf::Domain<bf::bv8>(8))
  {
    CHECK(CheckWeight(x));
    CHECK(CheckSize(x));
    CHECK(CheckZero(x));
    CHECK(CheckIO(x));

    CHECK(CheckWeight((bf::bv32)x));
    CHECK(CheckSize((bf::bv32)x));
    CHECK(CheckZero((bf::bv32)x));
    CHECK(CheckIO((bf::bv32)x));

    CHECK(CheckWeight((bf::bv64)x));
    CHECK(CheckSize((bf::bv64)x));
    CHECK(CheckZero((bf::bv64)x));
    CHECK(CheckIO((bf::bv64)x));
  }

  CHECK(CheckWrongIO());

  std::vector<bf::bv32> v32 = { BV32(0), BV32(1), BV32(2), BV32(5), BV32(7236414), BV32(45533423), BV32(442), BV32(897), BV32(12984), BV8(0xFFFFFFFF), BV8(0xFFFFFFF), BV8(0xFFFFFF) };

  for (auto x : v32)
  {
    CHECK(CheckWeight(x));
    CHECK(CheckSize(x));
    CHECK(CheckZero(x));
    CHECK(CheckIO(x));
  }

  for (auto x : v32)
  {
    for (auto y : v32)
    {
      bf::bv64 z = static_cast<bf::bv64>(x) + (static_cast<bf::bv64>(y) << 32);

      CHECK(CheckWeight(z));
      CHECK(CheckSize(z));
      CHECK(CheckZero(z));
      CHECK(CheckIO(z));
    }
  }

  std::vector<int> domDims = { 16, 17 };
  for (auto n : domDims)
  {
    CHECK(CheckDom(bf::Domain<bf::bv32>(n), n));
    CHECK(CheckDom(bf::Domain<bf::bv64>(n), n));
  }
 
  std::vector<bf::bv32> masks = { BV32(0xFF00FF), BV32(0x0101010F), BV32(0x01010110) };
  for (auto mask : masks)
  {
    CHECK(CheckMask(mask));
    CHECK(CheckMask(static_cast<bf::bv64>(mask)));
  }

  CHECK(CheckDeBruijn());

  CHECK(CheckLinearCombinations(0, true));
  CHECK(CheckLinearCombinations(0, false));

  CHECK(CheckLinearCombinations(5, true));
  CHECK(CheckLinearCombinations(5, false));

  CHECK(CheckLinearCombinations(9, true));
  CHECK(CheckLinearCombinations(9, false));

  std::cout << counter << " successful" << std::endl;
  
  return 0;
}
