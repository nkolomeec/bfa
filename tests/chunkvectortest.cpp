#include <vector>
#include <iostream>

#include "vect/chunkvector.h"
#include "vect/bvio.h"


unsigned int counter;

#define CHECK(x) if (!(x)) { std::cout << (counter + 1) << ": " << #x << " failed." << std::endl; return 1; } else { counter++; }

enum class Mode
{
  NotAligned = 0,
  Aligned = 1,
  ByBits = 2
};

bf::ChunkVector CreateVector(const std::vector<bf::bv32>& values, bf::bv8 m, Mode mode)
{
  bf::ChunkVector v(m * values.size());
  
    for (size_t i = 0; i < values.size(); ++i)
    {
      if (mode == Mode::NotAligned)
      {
        v.set(i * m, values[i], m);
      }
      else if (mode == Mode::Aligned)
      {
        v.setAligned(i * m, values[i], m);
      }
      else
      {
        for (bf::bv8 j = 0; j < m; ++j)
        {
          v.set(i * m + j, bf::get(values[i], j));
        }
      }
    }

  return v;
}

bool CheckVector(bf::ChunkVector v, const std::vector<bf::bv32> &values, bf::bv8 m)
{
  for (size_t i = 0; i < values.size(); ++i)
  {
    bf::bv32 result = v.get(i * m, m);

    if (result != values[i])
    {
      std::cout << "Getting by index " << i << " is failed" << std::endl;
      return false;
    }

    if (bf::zero((bf::bv8)(m & (m - 1))))
    {
      result = v.getAligned(i * m, m);

      if (result != values[i])
      {
        std::cout << "Aligned getting by index " << i << " is failed" << std::endl;
        return false;
      }
    }

    result = 0;

    for (bf::bv8 j = 0; j < m; ++j)
    {
      bf::set(result, j, v.get(i * m + j));
    }

    if (result != values[i])
    {
      std::cout << "Bitwise getting by index " << i << " is failed" << std::endl;
      return false;
    }
  }
  
  return true;
}

bool CheckByIndices(const std::vector<bf::bv32> &values, bf::bv8 m)
{
  if (bf::zero(m))
  {
    return false;
  }

  std::vector<Mode> modes{ Mode::NotAligned, Mode::ByBits };
  
  if (bf::zero((bf::bv8)(m & (m - 1))))
  {
    modes.push_back(Mode::Aligned);
  }

  for (auto mode : modes)
  {
    auto v = CreateVector(values, m, mode);

    if (!CheckVector(v, values, m))
    {
      std::cout << "Vector has been created in mode " << (int)mode << std::endl;
      return false;
    }
  }

  return true;
}


int main()
{
  CHECK(CheckByIndices({ 0, 1, 2 }, 2));

  CHECK(CheckByIndices({ 7, 6, 5, 4, 1, 2, 3, 0 }, 3));

  CHECK(CheckByIndices({ 7, 6, 5, 0, 4, 1, 2, 3, 15, 10, 9, 8, 14, 11, 12, 13 }, 4));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 5));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 6));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 7));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 8));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 9));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 10));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 11));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 12));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 13));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 14));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 15));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 16));

  CHECK(CheckByIndices({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 18, 20 }, 17));

  CHECK(CheckByIndices({ 0xFFFFFF, 0xFF0000 }, 24));

  CHECK(CheckByIndices({ BV32(0xFFFFFFFF), BV32(0xFF0000FF) }, 32));

  CHECK(CheckByIndices({ BV32(0xFFFFFFFF), BV32(0xFF0000FF), BV32(0xAABBCCFF) }, 32));

  std::cout << counter << " successful" << std::endl;

  return 0;
}
